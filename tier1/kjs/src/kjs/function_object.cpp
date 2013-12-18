// -*- c-basic-offset: 2 -*-
// krazy:excludeall=doublequote_chars (UStrings aren't QStrings)
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
 *  Copyright (C) 2003, 2004, 2005, 2006 Apple Computer, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "function_object.h"
#include "internal.h"
#include "function.h"
#include "scriptfunction.h"
#include "array_object.h"
#include "nodes.h"
#include "lexer.h"
#include "debugger.h"
#include "object.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

using namespace KJS;

// ------------------------------ FunctionPrototype -------------------------

FunctionPrototype::FunctionPrototype(ExecState *exec)
{
  static const Identifier* applyPropertyName = new Identifier("apply");
  static const Identifier* callPropertyName = new Identifier("call");
  static const Identifier* bindPropertyName = new Identifier("bind");

  putDirect(exec->propertyNames().length, jsNumber(0), DontDelete | ReadOnly | DontEnum);
  putDirectFunction(new FunctionProtoFunc(exec, this, FunctionProtoFunc::ToString, 0, exec->propertyNames().toString), DontEnum);
  putDirectFunction(new FunctionProtoFunc(exec, this, FunctionProtoFunc::Apply, 2, *applyPropertyName), DontEnum);
  putDirectFunction(new FunctionProtoFunc(exec, this, FunctionProtoFunc::Call, 1, *callPropertyName), DontEnum);
  putDirectFunction(new FunctionProtoFunc(exec, this, FunctionProtoFunc::Bind, 1, *bindPropertyName), DontEnum);
}

FunctionPrototype::~FunctionPrototype()
{
}

// ECMA 15.3.4
JSValue *FunctionPrototype::callAsFunction(ExecState * /*exec*/, JSObject * /*thisObj*/, const List &/*args*/)
{
  return jsUndefined();
}

// ------------------------------ FunctionProtoFunc -------------------------

FunctionProtoFunc::FunctionProtoFunc(ExecState* exec, FunctionPrototype* funcProto, int i, int len, const Identifier& name)
  : InternalFunctionImp(funcProto, name)
  , id(i)
{
  putDirect(exec->propertyNames().length, len, DontDelete | ReadOnly | DontEnum);
}

JSValue* FunctionProtoFunc::callAsFunction(ExecState* exec, JSObject* thisObj, const List &args)
{
  JSValue* result = NULL;

  switch (id) {
  case ToString:
    if (!thisObj || !thisObj->inherits(&InternalFunctionImp::info)) {
#ifndef NDEBUG
      fprintf(stderr,"attempted toString() call on null or non-function object\n");
#endif
      return throwError(exec, TypeError);
    }
    if (thisObj->inherits(&FunctionImp::info)) {
        return jsString(static_cast<FunctionImp*>(thisObj)->toSource());
     } else if (thisObj->inherits(&InternalFunctionImp::info) &&
                !static_cast<InternalFunctionImp*>(thisObj)->functionName().isNull()) {
       result = jsString("\nfunction " + static_cast<InternalFunctionImp*>(thisObj)->functionName().ustring() + "() {\n"
                       "    [native code]\n}\n");
    } else {
      result = jsString("[function]");
    }
    break;
  case Apply: {
    JSValue *thisArg = args[0];
    JSValue *argArray = args[1];
    JSObject *func = thisObj;

    if (!func->implementsCall())
      return throwError(exec, TypeError);

    JSObject *applyThis;
    if (thisArg->isUndefinedOrNull())
      applyThis = exec->dynamicInterpreter()->globalObject();
    else
      applyThis = thisArg->toObject(exec);

    List applyArgs;
    if (!argArray->isUndefinedOrNull()) {
      if (argArray->isObject() &&
           (static_cast<JSObject *>(argArray)->inherits(&ArrayInstance::info) ||
            static_cast<JSObject *>(argArray)->inherits(&Arguments::info))) {

        JSObject *argArrayObj = static_cast<JSObject *>(argArray);
        unsigned int length = argArrayObj->get(exec, exec->propertyNames().length)->toUInt32(exec);
        for (unsigned int i = 0; i < length; i++)
          applyArgs.append(argArrayObj->get(exec,i));
      }
      else
        return throwError(exec, TypeError);
    }
    result = func->call(exec,applyThis,applyArgs);
    }
    break;
  case Call: {
    JSValue *thisArg = args[0];
    JSObject *func = thisObj;

    if (!func->implementsCall())
      return throwError(exec, TypeError);

    JSObject *callThis;
    if (thisArg->isUndefinedOrNull())
      callThis = exec->dynamicInterpreter()->globalObject();
    else
      callThis = thisArg->toObject(exec);

    result = func->call(exec,callThis,args.copyTail());
    }
    break;
  case Bind: { //ECMA Edition 5.1r6 - 15.3.4.5
    JSObject* target(thisObj);
    if (!target->implementsCall())
      return throwError(exec, TypeError, "object is not callable");

    List newArgs;
    for (int i = 1; i < args.size(); ++i)
      newArgs.append(args[i]);

    JSObject* boundThis = 0;

    // As call does not accept JSValue(undefined/null),
    // do it like in call and use the global object
    if (args[0]->isUndefinedOrNull())
      boundThis = exec->dynamicInterpreter()->globalObject();
    else
      boundThis = args[0]->toObject(exec);

    BoundFunction* bfunc = new BoundFunction(exec, target, boundThis, newArgs);

    unsigned length;
    if (target->inherits(&FunctionImp::info)) {
      double L = target->get(exec, exec->propertyNames().length)->getNumber() - newArgs.size();
      length = (unsigned)std::max<int>((int)L, 0);
    } else {
      length = 0;
    }
    bfunc->put(exec, exec->propertyNames().length, jsNumber(length), ReadOnly|DontEnum|DontDelete);

    JSObject *thrower = new Thrower(TypeError);
    PropertyDescriptor callerDesc;

    GetterSetterImp* gs = new GetterSetterImp();
    gs->setGetter(thrower);
    gs->setSetter(thrower);

    callerDesc.setPropertyDescriptorValues(exec, gs, DontEnum|DontDelete);
    bfunc->defineOwnProperty(exec, exec->propertyNames().caller, callerDesc, false);

    PropertyDescriptor argumentsDesc;
    argumentsDesc.setPropertyDescriptorValues(exec, gs, DontEnum|DontDelete);
    bfunc->defineOwnProperty(exec, exec->propertyNames().arguments, argumentsDesc, false);

    return bfunc;
    }
    break;
  }

  return result;
}

// ------------------------------ FunctionObjectImp ----------------------------

FunctionObjectImp::FunctionObjectImp(ExecState* exec, FunctionPrototype* funcProto)
  : InternalFunctionImp(funcProto)
{
  putDirect(exec->propertyNames().prototype, funcProto, DontEnum|DontDelete|ReadOnly);

  // no. of arguments for constructor
  putDirect(exec->propertyNames().length, jsNumber(1), ReadOnly|DontDelete|DontEnum);
}

FunctionObjectImp::~FunctionObjectImp()
{
}

bool FunctionObjectImp::implementsConstruct() const
{
  return true;
}

// ECMA 15.3.2 The Function Constructor
JSObject* FunctionObjectImp::construct(ExecState* exec, const List& args, const Identifier& functionName, const UString& sourceURL, int lineNumber)
{
  UString p("");
  UString body;
  int argsSize = args.size();
  if (argsSize == 0) {
    body = "";
  } else if (argsSize == 1) {
    body = args[0]->toString(exec);
  } else {
    p = args[0]->toString(exec);
    for (int k = 1; k < argsSize - 1; k++)
      p += "," + args[k]->toString(exec);
    body = args[argsSize-1]->toString(exec);
  }

  // parse the source code
  int sourceId;
  int errLine;
  UString errMsg;
  RefPtr<FunctionBodyNode> functionBody = parser().parseFunctionBody(sourceURL, lineNumber, body.data(), body.size(), &sourceId, &errLine, &errMsg);

  // notify debugger that source has been parsed
  Debugger *dbg = exec->dynamicInterpreter()->debugger();
  if (dbg) {
    // make sure to pass in sourceURL, since it's useful for lazy event listeners, and empty for actual function ctor
    dbg->reportSourceParsed(exec, functionBody.get(), sourceId, sourceURL, body, lineNumber, errLine, errMsg);
  }

  // no program node == syntax error - throw a syntax error
  if (!functionBody)
    // we can't return a Completion(Throw) here, so just set the exception
    // and return it
    return throwError(exec, SyntaxError, errMsg, errLine, sourceId, sourceURL);

  ScopeChain scopeChain;
  scopeChain.push(exec->lexicalInterpreter()->globalObject());

  FunctionImp* fimp = new FunctionImp(exec, functionName, functionBody.get(), scopeChain);

  // parse parameter list. throw syntax error on illegal identifiers
  int len = p.size();
  const UChar *c = p.data();
  int i = 0, params = 0;
  UString param;
  while (i < len) {
      while (*c == ' ' && i < len)
          c++, i++;
      if (Lexer::isIdentStart(c->uc)) {  // else error
          param = UString(c, 1);
          c++, i++;
          while (i < len && (Lexer::isIdentPart(c->uc))) {
              param += UString(c, 1);
              c++, i++;
          }
          while (i < len && *c == ' ')
              c++, i++;
          if (i == len) {
              functionBody->addParam(Identifier(param));
              params++;
              break;
          } else if (*c == ',') {
              functionBody->addParam(Identifier(param));
              params++;
              c++, i++;
              continue;
          } // else error
      }
      return throwError(exec, SyntaxError, "Syntax error in parameter list");
  }

  List consArgs;

  JSObject* objCons = exec->lexicalInterpreter()->builtinObject();
  JSObject* prototype = objCons->construct(exec,List::empty());
  prototype->put(exec, exec->propertyNames().constructor, fimp, DontEnum|DontDelete|ReadOnly);
  // ECMA Edition 5.1r6 - 15.3.5.2 - [[Writable]]: true, [[Enumerable]]: false, [[Configurable]]: false
  fimp->put(exec, exec->propertyNames().prototype, prototype, Internal|DontDelete|DontEnum);
  return fimp;
}

// ECMA 15.3.2 The Function Constructor
JSObject* FunctionObjectImp::construct(ExecState* exec, const List& args)
{
  return construct(exec, args, "anonymous", UString(), 0);
}

// ECMA 15.3.1 The Function Constructor Called as a Function
JSValue* FunctionObjectImp::callAsFunction(ExecState* exec, JSObject* /*thisObj*/, const List &args)
{
  return construct(exec, args);
}
