// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2002 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003 Apple Computer, Inc.
 *  Copyright (C) 2007 Cameron Zwarich (cwzwarich@uwaterloo.ca)
 *  Copyright (C) 2007 Maksim Orlovich (maksim@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "function.h"
#include "scriptfunction.h"
#include "dtoa.h"
#include "internal.h"
#include "function_object.h"
#include "lexer.h"
#include "nodes.h"
#include "operations.h"
#include "debugger.h"
#include "PropertyNameArray.h"
#include "commonunicode.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <string>
#include "wtf/DisallowCType.h"
#include "wtf/ASCIICType.h"
#include "bytecode/machine.h"

using namespace WTF;

//#define KJS_VERBOSE

namespace KJS {

// ----------------------------- FunctionImp ----------------------------------

const ClassInfo FunctionImp::info = {"Function", &InternalFunctionImp::info, 0, 0};

FunctionImp::FunctionImp(ExecState* exec, const Identifier& n, FunctionBodyNode* b, const ScopeChain& sc)
  : InternalFunctionImp(static_cast<FunctionPrototype*>
                        (exec->lexicalInterpreter()->builtinFunctionPrototype()), n)
  , body(b)
  , _scope(sc)
{
}

void FunctionImp::mark()
{
    InternalFunctionImp::mark();
    _scope.mark();
}

FunctionImp::~FunctionImp()
{
}

void FunctionImp::initialCompile(ExecState* newExec)
{
    FunctionBodyNode* body = this->body.get();

    // Reserve various slots needed for the activation object. We do it only once,
    // --- isCompiled() would return true even if debugging state changed
    body->reserveSlot(ActivationImp::LengthSlot, false);
    body->reserveSlot(ActivationImp::TearOffNeeded, false);
    body->reserveSlot(ActivationImp::ScopeLink, false /* will mark via ScopeChain::mark() */);
    body->reserveSlot(ActivationImp::FunctionSlot, true);
    body->reserveSlot(ActivationImp::ArgumentsObjectSlot, true);

    // Create declarations for parameters, and allocate the symbols.
    // We always just give them sequential positions, to make passInParameters
    // simple (though perhaps wasting memory in the trivial case)
    for (size_t i = 0; i < body->numParams(); ++i)
      body->addSymbolOverwriteID(i + ActivationImp::NumReservedSlots, body->paramName(i), DontDelete);

    body->processDecls(newExec);
    body->compile(FunctionCode, newExec->dynamicInterpreter()->debugger() ? Debug : Release);
}


#ifdef KJS_VERBOSE
static int           callDepth;
static std::string   callIndent;

static const char* ind()
{
    callIndent = "";
    for (int i = 0; i < callDepth; ++i)
        callIndent += "     ";
    return callIndent.c_str();
}

// Multiline print adding indentation
static void printInd(const char* str)
{
    fprintf(stderr, "%s", ind());
    for (const char* c = str; *c; ++c) {
        if (*c != '\n')
            fprintf(stderr, "%c", *c);
        else
            fprintf(stderr, "\n%s", ind());
    }
}

#endif

JSValue* FunctionImp::callAsFunction(ExecState* exec, JSObject* thisObj, const List& args)
{
  assert(thisObj);

#ifdef KJS_VERBOSE
  ++callDepth;
#endif

  Debugger* dbg = exec->dynamicInterpreter()->debugger();

  // enter a new execution context
  FunctionExecState newExec(exec->dynamicInterpreter(), thisObj, body.get(), exec, this);
  if (exec->hadException())
    newExec.setException(exec->exception());

  FunctionBodyNode* body = this->body.get();

  // The first time we're called, compute the set of local variables,
  // and compile the body. (note that parameters have been collected
  // during the AST build)
  CompileType currentState = body->compileState();
  if (currentState == NotCompiled) {
    initialCompile(&newExec);
  } else {
    // Otherwise, we may still need to recompile due to debug...
    CompileType desiredState = dbg ? Debug : Release;
    if (desiredState != currentState)
        body->compile(FunctionCode, desiredState);
  }

  size_t stackSize              = 0;
  LocalStorageEntry* stackSpace = 0;

  // We always allocate on stack initially, and tearoff only after we're done.
  int regs   = body->numLocalsAndRegisters();
  stackSize  = sizeof(LocalStorageEntry) * regs;
  stackSpace = (LocalStorageEntry*)exec->dynamicInterpreter()->stackAlloc(stackSize);

  ActivationImp* activation = static_cast<ActivationImp*>(newExec.activationObject());
  activation->setup(&newExec, this, &args, stackSpace);
  activation->tearOffNeededSlot() = body->tearOffAtEnd();

  newExec.initLocalStorage(stackSpace, regs);

  JSValue* result = Machine::runBlock(&newExec, body->code(), exec);

  // If we need to tear off now --- either due to static flag above, or
  // if execution requested it dynamically --- do so now.
  if (activation->tearOffNeededSlot()) {
    activation->performTearOff();
  } else {
    // Otherwise, we recycle the activation object; we must clear its
    // data pointer, though, since that may become dead.
    // (we also unlink it from the scope chain at this time)
    activation->scopeLink().deref();
    activation->localStorage = 0;
    exec->dynamicInterpreter()->recycleActivation(activation);
  }

  // Now free the stack space..
  exec->dynamicInterpreter()->stackFree(stackSize);

#ifdef KJS_VERBOSE
  fprintf(stderr, "%s", ind());
  if (exec->exception())
    printInfo(exec,"throwing", exec->exception());
  else
    printInfo(exec,"returning", result);

  --callDepth;
#endif

  return result;
}

JSValue *FunctionImp::argumentsGetter(ExecState* exec, JSObject*, const Identifier& propertyName, const PropertySlot& slot)
{
  FunctionImp *thisObj = static_cast<FunctionImp *>(slot.slotBase());
  ExecState *context = exec;
  while (context) {
    if (context->function() == thisObj) {
      return static_cast<ActivationImp *>(context->activationObject())->get(exec, propertyName);
    }
    context = context->callingExecState();
  }
  return jsNull();
}

JSValue *FunctionImp::callerGetter(ExecState* exec, JSObject*, const Identifier&, const PropertySlot& slot)
{
    FunctionImp* thisObj = static_cast<FunctionImp*>(slot.slotBase());
    ExecState* context = exec;
    while (context) {
        if (context->function() == thisObj)
            break;
        context = context->callingExecState();
    }

    if (!context)
        return jsNull();

    ExecState* callingContext = context->callingExecState();
    if (!callingContext)
        return jsNull();

    FunctionImp* callingFunction = callingContext->function();
    if (!callingFunction)
        return jsNull();

    return callingFunction;
}

JSValue *FunctionImp::lengthGetter(ExecState*, JSObject*, const Identifier&, const PropertySlot& slot)
{
  FunctionImp *thisObj = static_cast<FunctionImp *>(slot.slotBase());
  return jsNumber(thisObj->body->numParams());
}

JSValue* FunctionImp::nameGetter(ExecState*, JSObject*, const Identifier&, const PropertySlot& slot)
{
    FunctionImp* thisObj = static_cast<FunctionImp*>(slot.slotBase());
    return jsString(thisObj->functionName().ustring());
}

bool FunctionImp::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    // Find the arguments from the closest context.
    if (propertyName == exec->propertyNames().arguments) {
        slot.setCustom(this, argumentsGetter);
        return true;
    }

    // Compute length of parameters.
    if (propertyName == exec->propertyNames().length) {
        slot.setCustom(this, lengthGetter);
        return true;
    }

    // Calling function (Mozilla-extension)
    if (propertyName == exec->propertyNames().caller) {
        slot.setCustom(this, callerGetter);
        return true;
    }

    // Function name (Mozilla-extension)
    if (propertyName == exec->propertyNames().name) {
        slot.setCustom(this, nameGetter);
        return true;
    }

    return InternalFunctionImp::getOwnPropertySlot(exec, propertyName, slot);
}

bool FunctionImp::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& desc)
{
    if (propertyName == exec->propertyNames().length) {
        desc.setPropertyDescriptorValues(exec, jsNumber(body->numParams()), ReadOnly|DontDelete|DontEnum);
        return true;
    }

    return KJS::JSObject::getOwnPropertyDescriptor(exec, propertyName, desc);
}

void FunctionImp::put(ExecState *exec, const Identifier &propertyName, JSValue *value, int attr)
{
    if (propertyName == exec->propertyNames().arguments ||
        propertyName == exec->propertyNames().length ||
        propertyName == exec->propertyNames().name)
        return;
    InternalFunctionImp::put(exec, propertyName, value, attr);
}

bool FunctionImp::deleteProperty(ExecState *exec, const Identifier &propertyName)
{
    if (propertyName == exec->propertyNames().arguments ||
        propertyName == exec->propertyNames().length ||
        propertyName == exec->propertyNames().name)
        return false;
    return InternalFunctionImp::deleteProperty(exec, propertyName);
}

/* Returns the parameter name corresponding to the given index. eg:
 * function f1(x, y, z): getParameterName(0) --> x
 *
 * If a name appears more than once, only the last index at which
 * it appears associates with it. eg:
 * function f2(x, x): getParameterName(0) --> null
 */
Identifier FunctionImp::getParameterName(size_t index)
{
  if (index >= body->numParams())
    return CommonIdentifiers::shared()->nullIdentifier;

  Identifier name = body->paramName(index);

  // Are there any subsequent parameters with the same name?
  for (size_t pos = index + 1; pos < body->numParams(); ++pos)
    if (body->paramName(pos) == name)
      return CommonIdentifiers::shared()->nullIdentifier;

  return name;
}

bool FunctionImp::implementsConstruct() const
{
  return true;
}

// ECMA 13.2.2 [[Construct]]
JSObject *FunctionImp::construct(ExecState *exec, const List &args)
{
  JSObject *proto;
  JSValue *p = get(exec, exec->propertyNames().prototype);
  if (p->isObject())
    proto = static_cast<JSObject*>(p);
  else
    proto = exec->lexicalInterpreter()->builtinObjectPrototype();

  JSObject *obj(new JSObject(proto));

  JSValue *res = call(exec,obj,args);

  if (res->isObject())
    return static_cast<JSObject *>(res);
  else
    return obj;
}

// ------------------------------ Thrower ---------------------------------

Thrower::Thrower(ErrorType type)
    : JSObject(),
      m_type(type)
{
}

JSValue* Thrower::callAsFunction(ExecState* exec, JSObject* /*thisObj*/, const List& /*args*/)
{
    return throwError(exec, m_type);
}


// ------------------------------ BoundFunction ---------------------------------

BoundFunction::BoundFunction(ExecState* exec, JSObject* targetFunction, JSObject* boundThis, KJS::List boundArgs)
    : InternalFunctionImp(static_cast<FunctionPrototype*>(exec->lexicalInterpreter()->builtinFunctionPrototype())),
      m_targetFunction(targetFunction),
      m_boundThis(boundThis),
      m_boundArgs(boundArgs)
{
}

// ECMAScript Edition 5.1r6 - 15.3.4.5.2
JSObject* BoundFunction::construct(ExecState* exec, const List& extraArgs)
{
    JSObject* target = m_targetFunction;
    if (!target->implementsConstruct())
        return throwError(exec, TypeError);
    List boundArgs = m_boundArgs;

    List args;
    for (int i = 0; i < boundArgs.size(); ++i)
        args.append(boundArgs.at(i));
    for (int i = 0; i < extraArgs.size(); ++i)
        args.append(extraArgs.at(i));

    return target->construct(exec, args);
}

// ECMAScript Edition 5.1r6 - 15.3.4.5.1
JSValue* BoundFunction::callAsFunction(ExecState* exec, JSObject* /*thisObj*/, const List& extraArgs)
{
    List boundArgs = m_boundArgs;
    JSObject* boundThis = m_boundThis;
    JSObject* target = m_targetFunction;

    List args;
    for (int i = 0; i < boundArgs.size(); ++i)
        args.append(boundArgs.at(i));
    for (int i = 0; i < extraArgs.size(); ++i)
        args.append(extraArgs.at(i));

    return target->callAsFunction(exec, boundThis, args);
}

// ECMAScript Edition 5.1r6 - 15.3.4.5.3
bool BoundFunction::hasInstance(ExecState* exec, JSValue* value)
{
    JSObject* target = m_targetFunction;
    if (!target->implementsHasInstance())
        return throwError(exec, TypeError);

    return target->hasInstance(exec, value);
}

void BoundFunction::setTargetFunction(JSObject* targetFunction)
{
    m_targetFunction = targetFunction;
}

void BoundFunction::setBoundArgs(const List& boundArgs)
{
    m_boundArgs = boundArgs;
}

void BoundFunction::setBoundThis(JSObject* boundThis)
{
    m_boundThis = boundThis;
}


// ------------------------------ IndexToNameMap ---------------------------------

// We map indexes in the arguments array to their corresponding argument names.
// Example: function f(x, y, z): arguments[0] = x, so we map 0 to Identifier("x").

// Once we have an argument name, we can get and set the argument's value in the
// activation object.

// We use Identifier::null to indicate that a given argument's value
// isn't stored in the activation object.

IndexToNameMap::IndexToNameMap(FunctionImp *func, const List &args)
{
  _map = new Identifier[args.size()];
  this->_size = args.size();

  size_t i = 0;
  ListIterator iterator = args.begin();
  for (; iterator != args.end(); i++, iterator++)
    _map[i] = func->getParameterName(i); // null if there is no corresponding parameter
}

IndexToNameMap::~IndexToNameMap() {
  delete [] _map;
}

bool IndexToNameMap::isMapped(const Identifier &index) const
{
  bool indexIsNumber;
  int indexAsNumber = index.toStrictUInt32(&indexIsNumber);

  if (!indexIsNumber)
    return false;

  if (indexAsNumber >= _size)
    return false;

  if (_map[indexAsNumber].isNull())
    return false;

  return true;
}

void IndexToNameMap::unMap(const Identifier &index)
{
  bool indexIsNumber;
  int indexAsNumber = index.toStrictUInt32(&indexIsNumber);

  assert(indexIsNumber && indexAsNumber < _size);

  _map[indexAsNumber] = CommonIdentifiers::shared()->nullIdentifier;;
}

int IndexToNameMap::size() const
{
    return _size;
}

Identifier& IndexToNameMap::operator[](int index)
{
  return _map[index];
}

Identifier& IndexToNameMap::operator[](const Identifier &index)
{
  bool indexIsNumber;
  int indexAsNumber = index.toStrictUInt32(&indexIsNumber);

  assert(indexIsNumber && indexAsNumber < _size);

  return (*this)[indexAsNumber];
}

// ------------------------------ Arguments ---------------------------------

const ClassInfo Arguments::info = {"Arguments", 0, 0, 0};

// ECMA 10.1.8
Arguments::Arguments(ExecState *exec, FunctionImp *func, const List &args, ActivationImp *act)
: JSObject(exec->lexicalInterpreter()->builtinObjectPrototype()),
_activationObject(act),
indexToNameMap(func, args)
{
  putDirect(exec->propertyNames().callee, func, DontEnum);
  putDirect(exec->propertyNames().length, args.size(), DontEnum);

  int i = 0;
  ListIterator iterator = args.begin();
  for (; iterator != args.end(); i++, iterator++) {
    if (!indexToNameMap.isMapped(Identifier::from(i))) {
      //ECMAScript Edition 5.1r6 - 10.6.11.b, [[Writable]]: true, [[Enumerable]]: true, [[Configurable]]: true
      JSObject::put(exec, Identifier::from(i), *iterator, None);
    }
  }
}

void Arguments::mark()
{
  JSObject::mark();
  if (_activationObject && !_activationObject->marked())
    _activationObject->mark();
}

JSValue *Arguments::mappedIndexGetter(ExecState* exec, JSObject*, const Identifier& propertyName, const PropertySlot& slot)
{
  Arguments *thisObj = static_cast<Arguments *>(slot.slotBase());
  return thisObj->_activationObject->get(exec, thisObj->indexToNameMap[propertyName]);
}

bool Arguments::getOwnPropertySlot(ExecState *exec, const Identifier& propertyName, PropertySlot& slot)
{
  if (indexToNameMap.isMapped(propertyName)) {
    slot.setCustom(this, mappedIndexGetter);
    return true;
  }

  return JSObject::getOwnPropertySlot(exec, propertyName, slot);
}

void Arguments::put(ExecState *exec, const Identifier &propertyName, JSValue *value, int attr)
{
  if (indexToNameMap.isMapped(propertyName)) {
    unsigned attr = 0;
    JSObject::getPropertyAttributes(propertyName, attr);
    if (attr & ReadOnly)
      return;

    _activationObject->put(exec, indexToNameMap[propertyName], value, attr);
  } else {
    JSObject::put(exec, propertyName, value, attr);
  }
}

bool Arguments::deleteProperty(ExecState *exec, const Identifier &propertyName)
{
  if (indexToNameMap.isMapped(propertyName)) {
    bool result = JSObject::deleteProperty(exec, propertyName);
    if (result) {
        _activationObject->deleteProperty(exec, indexToNameMap[propertyName]);
        indexToNameMap.unMap(propertyName);
    }
    return true;
  } else {
    return JSObject::deleteProperty(exec, propertyName);
  }
}

void Arguments::getOwnPropertyNames(ExecState* exec, PropertyNameArray& propertyNames, PropertyMap::PropertyMode mode)
{
    unsigned int length = indexToNameMap.size();
    unsigned attr;
    for (unsigned int i = 0; i < length; ++i) {
        attr = 0;
        Identifier ident = Identifier::from(i);

        if (indexToNameMap.isMapped(ident) &&
            _activationObject->getPropertyAttributes(indexToNameMap[ident], attr)) {
            if (PropertyMap::checkEnumerable(attr, mode)) {
                propertyNames.add(ident);
            }
        }
    }

    JSObject::getOwnPropertyNames(exec, propertyNames, mode);
}

bool Arguments::defineOwnProperty(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& desc, bool shouldThrow)
{
    bool isMapped = indexToNameMap.isMapped(propertyName);

    Identifier mappedName;
    if (isMapped)
        mappedName = indexToNameMap[propertyName];
    else
        mappedName = propertyName;

    bool allowed = JSObject::defineOwnProperty(exec, propertyName, desc, false);

    if (!allowed) {
        if (shouldThrow)
            throwError(exec, TypeError);
        return false;
    }
    if (isMapped) {
        if (desc.isAccessorDescriptor()) {
            indexToNameMap.unMap(propertyName);
        } else {
            if (desc.value()) {
                _activationObject->putDirect(mappedName, desc.value(), desc.attributes());
            }
            if (desc.writableSet() && desc.writable() == false) {
                indexToNameMap.unMap(propertyName);
            }
        }
    }

    return true;
}

// ------------------------------ ActivationImp --------------------------------

const ClassInfo ActivationImp::info = {"Activation", 0, 0, 0};

// ECMA 10.1.6
void ActivationImp::setup(ExecState* exec, FunctionImp *function,
                          const List* arguments, LocalStorageEntry* entries)
{
    FunctionBodyNode* body = function->body.get();

    size_t total = body->numLocalsAndRegisters();
    localStorage  = entries;
    lengthSlot()  = total;

    // we can now link ourselves into the scope, which will also fix up our scopeLink().
    exec->pushVariableObjectScope(this);

    const FunctionBodyNode::SymbolInfo* symInfo = body->getLocalInfo();

    // Setup our fields
    this->arguments = arguments;
    functionSlot()  = function;
    argumentsObjectSlot() = jsUndefined();
    symbolTable     = &body->symbolTable();

    // Set the mark/don't mark flags and attributes for everything
    for (size_t p = 0; p < total; ++p)
      entries[p].attributes = symInfo[p].attr;

    // Pass in the parameters (ECMA 10.1.3q)
#ifdef KJS_VERBOSE
    fprintf(stderr, "%s---------------------------------------------------\n"
            "%sprocessing parameters for %s call\n", ind(), ind(), 
            function->functionName().isEmpty() ? "(internal)" : function->functionName().ascii());
#endif
    size_t numParams   = body->numParams();
    size_t numPassedIn = min(numParams, static_cast<size_t>(arguments->size()));

    size_t pos = 0; 
    for (; pos < numPassedIn; ++pos) {
        size_t symNum = pos + ActivationImp::NumReservedSlots;
        JSValue* v = arguments->atUnchecked(pos);

        entries[symNum].val.valueVal = v;

#ifdef KJS_VERBOSE
        fprintf(stderr, "%s setting parameter %s", ind(), body->paramName(pos).ascii());
        printInfo(exec, "to", v);
#endif
    }

    for (; pos < numParams; ++pos) {
        size_t symNum = pos + ActivationImp::NumReservedSlots;
        entries[symNum].val.valueVal = jsUndefined();

#ifdef KJS_VERBOSE
        fprintf(stderr, "%s setting parameter %s to undefined (not passed in)", ind(), body->paramName(pos).ascii());
#endif
    }
    
#ifdef KJS_VERBOSE
    fprintf(stderr, "\n%s---------------------------------\n", ind());
    fprintf(stderr, "%sBody:\n", ind());
    fprintf(stderr, "%s---------------------------------\n", ind());
    printInd(body->toString().ascii());
    fprintf(stderr, "\n%s---------------------------------\n\n", ind());
#endif

    // Initialize the rest of the locals to 'undefined'
    for (size_t pos = numParams + ActivationImp::NumReservedSlots; pos < total; ++pos)
        entries[pos].val.valueVal = jsUndefined();

    // Finally, put in the functions. Note that this relies on above
    // steps to have completed, since it can trigger a GC.
    size_t  numFuns  = body->numFunctionLocals();
    size_t* funsData = body->getFunctionLocalInfo();
    for (size_t fun = 0; fun < numFuns; ++fun) {
        size_t id = funsData[fun];
        entries[id].val.valueVal = symInfo[id].funcDecl->makeFunctionObject(exec);
    }
}

void ActivationImp::performTearOff()
{
    // Create a new local array, copy stuff over
    size_t total = lengthSlot();
    LocalStorageEntry* entries = new LocalStorageEntry[total];
    std::memcpy(entries, localStorage, total*sizeof(LocalStorageEntry));
    localStorage  = entries;
}

void ActivationImp::requestTearOff()
{
    tearOffNeededSlot() = true;
}

JSValue *ActivationImp::argumentsGetter(ExecState* exec, JSObject*, const Identifier&, const PropertySlot& slot)
{
  ActivationImp* thisObj = static_cast<ActivationImp*>(slot.slotBase());

  if (thisObj->argumentsObjectSlot() == jsUndefined())
    thisObj->createArgumentsObject(exec);

  return thisObj->argumentsObjectSlot();
}


PropertySlot::GetValueFunc ActivationImp::getArgumentsGetter()
{
  return ActivationImp::argumentsGetter;
}

bool ActivationImp::getOwnPropertySlot(ExecState *exec, const Identifier& propertyName, PropertySlot& slot)
{
    if (symbolTableGet(propertyName, slot))
        return true;

    if (JSValue** location = getDirectLocation(propertyName)) {
        slot.setValueSlot(this, location);
        return true;
    }

    // Only return the built-in arguments object if it wasn't overridden above.
    if (propertyName == exec->propertyNames().arguments) {
        slot.setCustom(this, getArgumentsGetter());
        return true;
    }

    // We don't call through to JSObject because there's no way to give an
    // activation object getter properties or a prototype.
    ASSERT(!_prop.hasGetterSetterProperties());
    ASSERT(prototype() == jsNull());
    return false;
}

bool ActivationImp::deleteProperty(ExecState *exec, const Identifier &propertyName)
{
    if (propertyName == exec->propertyNames().arguments)
        return false;

    return JSVariableObject::deleteProperty(exec, propertyName);
}

void ActivationImp::putDirect(const Identifier& propertyName, JSValue* value, int attr)
{
    size_t index = symbolTable->get(propertyName.ustring().rep());
    if (index != missingSymbolMarker()) {
        LocalStorageEntry& entry = localStorage[index];
        entry.val.valueVal = value;
        entry.attributes = attr;
        return;
    }

    JSVariableObject::putDirect(propertyName, value, attr);
}

JSValue* ActivationImp::getDirect(const Identifier& propertyName) const
{
    size_t index = symbolTable->get(propertyName.ustring().rep());
    if (index != missingSymbolMarker()) {
        LocalStorageEntry& entry = localStorage[index];
        return entry.val.valueVal;
    }

    return JSVariableObject::getDirect(propertyName);
}

bool ActivationImp::getPropertyAttributes(const Identifier& propertyName, unsigned int& attributes) const
{
    size_t index = symbolTable->get(propertyName.ustring().rep());
    if (index != missingSymbolMarker()) {
        LocalStorageEntry& entry = localStorage[index];
        attributes = entry.attributes;
        return true;
    }

    return JSVariableObject::getPropertyAttributes(propertyName, attributes);
}

void ActivationImp::put(ExecState*, const Identifier& propertyName, JSValue* value, int attr)
{
    // If any bits other than DontDelete are set, then we bypass the read-only check.
    bool checkReadOnly = !(attr & ~DontDelete);
    if (symbolTablePut(propertyName, value, checkReadOnly))
        return;

    // We don't call through to JSObject because __proto__ and getter/setter
    // properties are non-standard extensions that other implementations do not
    // expose in the activation object.
    ASSERT(!_prop.hasGetterSetterProperties());
    _prop.put(propertyName, value, attr, checkReadOnly);
}

void ActivationImp::createArgumentsObject(ExecState *exec)
{
    requestTearOff();
    argumentsObjectSlot() = new Arguments(exec, static_cast<FunctionImp*>(functionSlot()),
                                          *arguments, const_cast<ActivationImp*>(this));
}

// ------------------------------ GlobalFunc -----------------------------------


GlobalFuncImp::GlobalFuncImp(ExecState* exec, FunctionPrototype* funcProto, int i, int len, const Identifier& name)
  : InternalFunctionImp(funcProto, name)
  , id(i)
{
  putDirect(exec->propertyNames().length, len, DontDelete|ReadOnly|DontEnum);
}

static JSValue *encode(ExecState *exec, const List &args, const char *do_not_escape)
{
  UString r = "", s, str = args[0]->toString(exec);
  CString cstr = str.UTF8String();
  const char *p = cstr.c_str();
  for (size_t k = 0; k < cstr.size(); k++, p++) {
    char c = *p;
    if (c && strchr(do_not_escape, c)) {
      r.append(c);
    } else {
      char tmp[4];
      sprintf(tmp, "%%%02X", (unsigned char)c);
      r += tmp;
    }
  }
  return jsString(r);
}

static JSValue *decode(ExecState *exec, const List &args, const char *do_not_unescape)
{
  UString s = "", str = args[0]->toString(exec);
  int k = 0, len = str.size();
  const UChar *d = str.data();
  UChar u;
  while (k < len) {
    const UChar *p = d + k;
    UChar c = *p;
    if (c == '%') {
      int charLen = 0;
      if (k <= len - 3 && isASCIIHexDigit(p[1].uc) && isASCIIHexDigit(p[2].uc)) {
        const char b0 = Lexer::convertHex(p[1].uc, p[2].uc);
        const int sequenceLen = UTF8SequenceLength(b0);
        if (sequenceLen != 0 && k <= len - sequenceLen * 3) {
          charLen = sequenceLen * 3;
          char sequence[5];
          sequence[0] = b0;
          for (int i = 1; i < sequenceLen; ++i) {
            const UChar *q = p + i * 3;
            if (q[0] == '%' && isASCIIHexDigit(q[1].uc) && isASCIIHexDigit(q[2].uc))
              sequence[i] = Lexer::convertHex(q[1].uc, q[2].uc);
            else {
              charLen = 0;
              break;
            }
          }
          if (charLen != 0) {
            sequence[sequenceLen] = 0;
            const int character = decodeUTF8Sequence(sequence);
            if (character < 0 || character >= 0x110000) {
              charLen = 0;
            } else if (character >= 0x10000) {
              // Convert to surrogate pair.
              s.append(static_cast<unsigned short>(0xD800 | ((character - 0x10000) >> 10)));
              u = static_cast<unsigned short>(0xDC00 | ((character - 0x10000) & 0x3FF));
            } else {
              u = static_cast<unsigned short>(character);
            }
          }
        }
      }
      if (charLen == 0)
          return throwError(exec, URIError);
      if (u.uc == 0 || u.uc >= 128 || !strchr(do_not_unescape, u.low())) {
        c = u;
        k += charLen - 1;
      }
    }
    k++;
    s.append(c);
  }
  return jsString(s);
}

static int parseDigit(unsigned short c, int radix)
{
    int digit = -1;

    if (c >= '0' && c <= '9') {
        digit = c - '0';
    } else if (c >= 'A' && c <= 'Z') {
        digit = c - 'A' + 10;
    } else if (c >= 'a' && c <= 'z') {
        digit = c - 'a' + 10;
    }

    if (digit >= radix)
        return -1;
    return digit;
}

double parseIntOverflow(const char* s, int length, int radix)
{
    double number = 0.0;
    double radixMultiplier = 1.0;

    for (const char* p = s + length - 1; p >= s; p--) {
        if (radixMultiplier == Inf) {
            if (*p != '0') {
                number = Inf;
                break;
            }
        } else {
            int digit = parseDigit(*p, radix);
            number += digit * radixMultiplier;
        }

        radixMultiplier *= radix;
    }

    return number;
}

static double parseInt(const UString &s, int radix)
{
    int length = s.size();
    int p = 0;

    while (p < length && CommonUnicode::isStrWhiteSpace(s[p].uc)) {
        ++p;
    }

    double sign = 1;
    if (p < length) {
        if (s[p] == '+') {
            ++p;
        } else if (s[p] == '-') {
            sign = -1;
            ++p;
        }
    }

    if ((radix == 0 || radix == 16) && length - p >= 2 && s[p] == '0' && (s[p + 1] == 'x' || s[p + 1] == 'X')) {
        radix = 16;
        p += 2;
    } else if (radix == 0) {
        // ECMAscript test262 S15.1.2.2_A5.1_T1 says we should no longer accept octal. To fix remove next 3 lines.
        if (p < length && s[p] == '0')
            radix = 8;
        else
            radix = 10;
    }

    if (radix < 2 || radix > 36)
        return NaN;

    int firstDigitPosition = p;
    bool sawDigit = false;
    double number = 0;
    while (p < length) {
        int digit = parseDigit(s[p].uc, radix);
        if (digit == -1)
            break;
        sawDigit = true;
        number *= radix;
        number += digit;
        ++p;
    }

    if (number >= mantissaOverflowLowerBound) {
        if (radix == 10)
            number = kjs_strtod(s.substr(firstDigitPosition, p - firstDigitPosition).ascii(), 0);
        else if (radix == 2 || radix == 4 || radix == 8 || radix == 16 || radix == 32)
            number = parseIntOverflow(s.substr(firstDigitPosition, p - firstDigitPosition).ascii(), p - firstDigitPosition, radix);
    }

    if (!sawDigit)
        return NaN;

    return sign * number;
}

static double parseFloat(const UString &s)
{
    // Check for 0x prefix here, because toDouble allows it, but we must treat it as 0.
    // Need to skip any whitespace and then one + or - sign.
    int length = s.size();
    int p = 0;
    while (p < length && CommonUnicode::isStrWhiteSpace(s[p].uc)) {
        ++p;
    }
    if (p < length && (s[p] == '+' || s[p] == '-')) {
        ++p;
    }
    if (length - p >= 2 && s[p] == '0' && (s[p + 1] == 'x' || s[p + 1] == 'X')) {
        return 0;
    }

    return s.toDouble( true /*tolerant*/, false /* NaN for empty string */ );
}

JSValue *GlobalFuncImp::callAsFunction(ExecState *exec, JSObject * /*thisObj*/, const List &args)
{
  JSValue *res = jsUndefined();

  static const char do_not_escape[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "*+-./@_";

  static const char do_not_escape_when_encoding_URI_component[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "!'()*-._~";
  static const char do_not_escape_when_encoding_URI[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "!#$&'()*+,-./:;=?@_~";
  static const char do_not_unescape_when_decoding_URI[] =
    "#$&+,/:;=?@";

  switch (id) {
    case Eval: { // eval()
      JSValue *x = args[0];
      if (!x->isString())
        return x;
      else {
        UString s = x->toString(exec);

        int sourceId;
        int errLine;
        UString errMsg;
        RefPtr<ProgramNode> progNode(parser().parseProgram(UString(), 0, s.data(), s.size(), &sourceId, &errLine, &errMsg));

        Debugger *dbg = exec->dynamicInterpreter()->debugger();
        if (dbg) {
          dbg->reportSourceParsed(exec, progNode.get(), sourceId, UString(), s, 0, errLine, errMsg);
        }

        // no program node means a syntax occurred
        if (!progNode)
          return throwError(exec, SyntaxError, errMsg, errLine, sourceId, NULL);

        // If the variable object we're working with is an activation, we better
        // tear it off since stuff inside eval can capture it in a closure
        if (exec->variableObject()->isActivation())
            static_cast<ActivationImp*>(exec->variableObject())->requestTearOff();

        // enter a new execution context
        EvalExecState newExec(exec->dynamicInterpreter(),
                       exec->dynamicInterpreter()->globalObject(),
                       progNode.get(),
                       exec);

        if (exec->hadException())
            newExec.setException(exec->exception());

        if (dbg) {
            bool cont = dbg->enterContext(&newExec, sourceId, 0, 0, List::empty());
            if (!cont) {
                dbg->imp()->abort();
                return jsUndefined();
            }
        }

        // execute the code
        progNode->processDecls(&newExec);
        Completion c = progNode->execute(&newExec);

        dbg = exec->dynamicInterpreter()->debugger();
        if (dbg) {
            bool cont = dbg->exitContext(&newExec, sourceId, 0, 0);
            if (!cont) {
                dbg->imp()->abort();
                return jsUndefined();
            }
        }

        // if an exception occurred, propagate it back to the previous execution object
        if (newExec.hadException())
          exec->setException(newExec.exception());

        res = jsUndefined();
        if (c.complType() == Throw)
          exec->setException(c.value());
        else if (c.isValueCompletion())
            res = c.value();
      }
      break;
    }
  case ParseInt:
    res = jsNumber(parseInt(args[0]->toString(exec), args[1]->toInt32(exec)));
    break;
  case ParseFloat:
    res = jsNumber(parseFloat(args[0]->toString(exec)));
    break;
  case IsNaN:
    res = jsBoolean(isNaN(args[0]->toNumber(exec)));
    break;
  case IsFinite: {
    double n = args[0]->toNumber(exec);
    res = jsBoolean(!isNaN(n) && !isInf(n));
    break;
  }
  case DecodeURI:
    res = decode(exec, args, do_not_unescape_when_decoding_URI);
    break;
  case DecodeURIComponent:
    res = decode(exec, args, "");
    break;
  case EncodeURI:
    res = encode(exec, args, do_not_escape_when_encoding_URI);
    break;
  case EncodeURIComponent:
    res = encode(exec, args, do_not_escape_when_encoding_URI_component);
    break;
  case Escape:
    {
      UString r = "", s, str = args[0]->toString(exec);
      const UChar* c = str.data();
      for (int k = 0; k < str.size(); k++, c++) {
        int u = c->uc;
        if (u > 255) {
          char tmp[7];
          sprintf(tmp, "%%u%04X", u);
          s = UString(tmp);
        } else if (u != 0 && strchr(do_not_escape, (char)u)) {
          s = UString(c, 1);
        } else {
          char tmp[4];
          sprintf(tmp, "%%%02X", u);
          s = UString(tmp);
        }
        r += s;
      }
      res = jsString(r);
      break;
    }
  case UnEscape:
    {
      UString s = "", str = args[0]->toString(exec);
      int k = 0, len = str.size();
      while (k < len) {
        const UChar* c = str.data() + k;
        UChar u;
        if (*c == UChar('%') && k <= len - 6 && *(c+1) == UChar('u')) {
          if (Lexer::isHexDigit((c+2)->uc) && Lexer::isHexDigit((c+3)->uc) &&
              Lexer::isHexDigit((c+4)->uc) && Lexer::isHexDigit((c+5)->uc)) {
	  u = Lexer::convertUnicode((c+2)->uc, (c+3)->uc,
				    (c+4)->uc, (c+5)->uc);
	  c = &u;
	  k += 5;
          }
        } else if (*c == UChar('%') && k <= len - 3 &&
                   Lexer::isHexDigit((c+1)->uc) && Lexer::isHexDigit((c+2)->uc)) {
          u = UChar(Lexer::convertHex((c+1)->uc, (c+2)->uc));
          c = &u;
          k += 2;
        }
        k++;
        s += UString(c, 1);
      }
      res = jsString(s);
      break;
    }
#ifndef NDEBUG
  case KJSPrint:
    puts(args[0]->toString(exec).ascii());
    break;
#endif
  }

  return res;
}

} // namespace

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
