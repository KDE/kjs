// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003, 2004, 2005, 2006 Apple Computer, Inc.
 *  Copyright (C) 2007 Eric Seidel (eric@webkit.org)
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

#include "object.h"

#include "error_object.h"
#include "lookup.h"
#include "nodes.h"
#include "operations.h"
#include "PropertyNameArray.h"
#include <math.h>

#include <typeinfo>

#define JAVASCRIPT_MARK_TRACING 0

namespace KJS {

// ------------------------------ JSObject ------------------------------------

void JSObject::mark()
{
  JSCell::mark();

#if JAVASCRIPT_MARK_TRACING
  static int markStackDepth = 0;
  markStackDepth++;
  for (int i = 0; i < markStackDepth; i++)
    putchar('-');

  printf("%s (%p)\n", className().UTF8String().c_str(), this);
#endif

  JSValue *proto = _proto;
  if (!proto->marked())
    proto->mark();

  _prop.mark();

#if JAVASCRIPT_MARK_TRACING
  markStackDepth--;
#endif
}

JSType JSObject::type() const
{
  return ObjectType;
}

const ClassInfo *JSObject::classInfo() const
{
  return 0;
}

UString JSObject::className() const
{
  const ClassInfo *ci = classInfo();
  return ci ? ci->className : "Object";
}

JSValue *JSObject::get(ExecState *exec, const Identifier &propertyName) const
{
  PropertySlot slot;

  if (const_cast<JSObject *>(this)->getPropertySlot(exec, propertyName, slot)) {
	JSValue* val = slot.getValue(exec, const_cast<JSObject *>(this), propertyName);
	assert(val);
	return val;
  }

  return jsUndefined();
}

JSValue *JSObject::get(ExecState *exec, unsigned propertyName) const
{
  PropertySlot slot;
  if (const_cast<JSObject *>(this)->getPropertySlot(exec, propertyName, slot))
    return slot.getValue(exec, const_cast<JSObject *>(this), propertyName);

  return jsUndefined();
}

bool JSObject::getPropertySlot(ExecState *exec, unsigned propertyName, PropertySlot& slot)
{
  JSObject *imp = this;

  while (true) {
    if (imp->getOwnPropertySlot(exec, propertyName, slot))
      return true;

    JSValue *proto = imp->_proto;
    if (!proto->isObject())
      break;

    imp = static_cast<JSObject *>(proto);
  }

  return false;
}

bool JSObject::getPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& desc)
{
    JSObject* object = this;
    while (true) {
        if (object->getOwnPropertyDescriptor(exec, propertyName, desc))
            return true;
        JSValue *prototype = object->prototype();
        if (!prototype->isObject())
            return false;
        object = prototype->toObject(exec);
    }
}

bool JSObject::getOwnPropertySlot(ExecState *exec, unsigned propertyName, PropertySlot& slot)
{
  return getOwnPropertySlot(exec, Identifier::from(propertyName), slot);
}

// Ideally, we would like to inline this, since it's ultra-hot, but with the large VM
// loop, it seems like the code side gets the g++-4.3.x inliner in the paranoid mode, so not only
// does it not inline this, but it also doesn't inline setValueSlot() and hasGetterSetterProperties() (!!!).
bool JSObject::getOwnPropertySlot(ExecState *exec, const Identifier& propertyName, PropertySlot& slot)
{
    if (JSValue **location = getDirectLocation(propertyName)) {
        if (_prop.hasGetterSetterProperties() && location[0]->type() == GetterSetterType)
            fillGetterPropertySlot(slot, location);
        else
            slot.setValueSlot(this, location);
        return true;
    }

    // non-standard Netscape extension
    if (propertyName == exec->propertyNames().underscoreProto) {
        slot.setValueSlot(this, &_proto);
        return true;
    }

    return false;
}

bool JSObject::getOwnPropertyDescriptor(ExecState* exec, const Identifier& identifier, PropertyDescriptor& desc)
{
    JSValue* jsVal = getDirect(identifier);

    // for classes that do not implement getDirect, like the prototypes,
    // we have to check if they still do own the property and use the propertyslot
    if (!jsVal) {
        PropertySlot slot;
        if (getOwnPropertySlot(exec, identifier, slot))
            jsVal = slot.getValue(exec, this, identifier);
    }

    if (jsVal) {
        unsigned attr = 0;
        getPropertyAttributes(identifier, attr);
        return desc.setPropertyDescriptorValues(exec, jsVal, attr);
    }
    return false;
}

static void throwSetterError(ExecState *exec)
{
  throwError(exec, TypeError, "setting a property that has only a getter");
}

// ECMA 8.6.2.2
void JSObject::put(ExecState *exec, const Identifier &propertyName, JSValue *value, int attr)
{
  assert(value);

  // non-standard netscape extension
  if (propertyName == exec->propertyNames().underscoreProto) {
    JSObject* proto = value->getObject();
    while (proto) {
      if (proto == this) {
        throwError(exec, GeneralError, "cyclic __proto__ value");
        return;
      }
      proto = proto->prototype() ? proto->prototype()->getObject() : 0;
    }

    setPrototype(value);
    return;
  }

  // putValue() is used for JS assignemnts. It passes no attribute.
  // Assume that a C++ implementation knows what it is doing
  // and don't spend time doing a read-only check for it.
  bool checkRO = (attr == None || attr == DontDelete);

  if (checkRO) {
    // Check for static properties that are ReadOnly; the property map will check the dynamic properties.
    // We don't have to worry about setters being read-only as they can't be added with such an attribute.
    // We also need to inherit any attributes we have from the entry
    const HashEntry* entry = findPropertyHashEntry(propertyName);
    if (entry) {
      if (entry->attr & ReadOnly) {
#ifdef KJS_VERBOSE
        fprintf( stderr, "WARNING: static property %s is ReadOnly\n", propertyName.ascii() );
#endif
        return;
      }
      attr = entry->attr;
    }
  }

  // Check if there are any setters or getters in the prototype chain
  JSObject *obj = this;
  bool hasGettersOrSetters = false;
  while (true) {
    if (obj->_prop.hasGetterSetterProperties()) {
      hasGettersOrSetters = true;
      break;
    }

    if (!obj->_proto->isObject())
      break;

    obj = static_cast<JSObject *>(obj->_proto);
  }

  if (hasGettersOrSetters) {
    obj = this;
    while (true) {
      unsigned attributes;
      if (JSValue *gs = obj->_prop.get(propertyName, attributes)) {
        if (attributes & GetterSetter) {
          JSObject *setterFunc = static_cast<GetterSetterImp *>(gs)->getSetter();

          if (!setterFunc) {
            if (false) //only throw if strict is set
              throwSetterError(exec);
            return;
          }

          List args;
          args.append(value);

          setterFunc->call(exec, this, args);
          return;
        } else {
          // If there's an existing property on the object or one of its
          // prototype it should be replaced, so we just break here.
          break;
        }
      }

      if (!obj->_proto->isObject())
        break;

      obj = static_cast<JSObject *>(obj->_proto);
    }
  }

  if (!isExtensible() && !_prop.get(propertyName))
      return;
  _prop.put(propertyName,value,attr,checkRO);
}

void JSObject::put(ExecState *exec, unsigned propertyName,
                     JSValue *value, int attr)
{
  put(exec, Identifier::from(propertyName), value, attr);
}

// ECMA 8.6.2.3
bool JSObject::canPut(ExecState *, const Identifier &propertyName) const
{
  unsigned attributes;

  // Don't look in the prototype here. We can always put an override
  // in the object, even if the prototype has a ReadOnly property.

  if (!getPropertyAttributes(propertyName, attributes))
    return true;
  else
    return !(attributes & ReadOnly);
}

// ECMA 8.6.2.4
bool JSObject::hasProperty(ExecState *exec, const Identifier &propertyName) const
{
  PropertySlot slot;
  return const_cast<JSObject *>(this)->getPropertySlot(exec, propertyName, slot);
}

bool JSObject::hasProperty(ExecState *exec, unsigned propertyName) const
{
  PropertySlot slot;
  return const_cast<JSObject *>(this)->getPropertySlot(exec, propertyName, slot);
}

// ECMA 8.6.2.5
bool JSObject::deleteProperty(ExecState * /*exec*/, const Identifier &propertyName)
{
  unsigned attributes;
  JSValue *v = _prop.get(propertyName, attributes);
  if (v) {
    if ((attributes & DontDelete))
      return false;
    _prop.remove(propertyName);
    if (attributes & GetterSetter)
        _prop.setHasGetterSetterProperties(_prop.containsGettersOrSetters());
    return true;
  }

  // Look in the static hashtable of properties
  const HashEntry* entry = findPropertyHashEntry(propertyName);
  if (entry && entry->attr & DontDelete)
    return false; // this builtin property can't be deleted
  return true;
}

bool JSObject::deleteProperty(ExecState *exec, unsigned propertyName)
{
  return deleteProperty(exec, Identifier::from(propertyName));
}

static ALWAYS_INLINE JSValue *tryGetAndCallProperty(ExecState *exec, const JSObject *object, const Identifier &propertyName) {
  JSValue *v = object->get(exec, propertyName);
  if (v->isObject()) {
    JSObject *o = static_cast<JSObject*>(v);
    if (o->implementsCall()) { // spec says "not primitive type" but ...
      JSObject *thisObj = const_cast<JSObject*>(object);
      JSValue *def = o->call(exec, thisObj, List::empty());
      JSType defType = def->type();
      ASSERT(defType != GetterSetterType);
      if (defType != ObjectType)
        return def;
    }
  }
  return NULL;
}

bool JSObject::getPrimitiveNumber(ExecState* exec, double& number, JSValue*& result)
{
    result = defaultValue(exec, NumberType);
    number = result->toNumber(exec);
    return !result->isString();
}

// ECMA 8.6.2.6
JSValue *JSObject::defaultValue(ExecState *exec, JSType hint) const
{
  const Identifier* firstPropertyName;
  const Identifier* secondPropertyName;
  /* Prefer String for Date objects */
  if ((hint == StringType) || ((hint != NumberType) && (_proto == exec->lexicalInterpreter()->builtinDatePrototype()))) {
    firstPropertyName = &exec->propertyNames().toString;
    secondPropertyName = &exec->propertyNames().valueOf;
  } else {
    firstPropertyName = &exec->propertyNames().valueOf;
    secondPropertyName = &exec->propertyNames().toString;
  }

  JSValue *v;
  if ((v = tryGetAndCallProperty(exec, this, *firstPropertyName)))
    return v;
  if ((v = tryGetAndCallProperty(exec, this, *secondPropertyName)))
    return v;

  if (exec->hadException())
    return exec->exception();

  return throwError(exec, TypeError, "No default value");
}

const HashEntry* JSObject::findPropertyHashEntry(const Identifier& propertyName) const
{
  for (const ClassInfo *info = classInfo(); info; info = info->parentClass) {
    if (const HashTable *propHashTable = info->propHashTable) {
      if (const HashEntry *e = Lookup::findEntry(propHashTable, propertyName))
        return e;
    }
  }
  return 0;
}

void JSObject::defineGetter(ExecState*, const Identifier& propertyName, JSObject* getterFunc)
{
    JSValue *o = getDirect(propertyName);
    GetterSetterImp *gs;

    if (o && o->type() == GetterSetterType) {
        gs = static_cast<GetterSetterImp *>(o);
    } else {
        gs = new GetterSetterImp;
        putDirect(propertyName, gs, GetterSetter);
    }

    _prop.setHasGetterSetterProperties(true);
    gs->setGetter(getterFunc);
}

void JSObject::defineSetter(ExecState*, const Identifier& propertyName, JSObject* setterFunc)
{
    JSValue *o = getDirect(propertyName);
    GetterSetterImp *gs;

    if (o && o->type() == GetterSetterType) {
        gs = static_cast<GetterSetterImp *>(o);
    } else {
        gs = new GetterSetterImp;
        putDirect(propertyName, gs, GetterSetter);
    }

    _prop.setHasGetterSetterProperties(true);
    gs->setSetter(setterFunc);
}

//ECMA Edition 5.1r6 - 8.12.9
bool JSObject::defineOwnProperty(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& desc, bool shouldThrow)
{
    PropertyDescriptor current;

    // if Object does not have propertyName as OwnProperty just push it.
    if (!getOwnPropertyDescriptor(exec, propertyName, current)) {
        if (!isExtensible()) {
            if (shouldThrow)
                throwError(exec, TypeError, "Object is not extensible \'" + propertyName.ustring() + "\'");
            return false;
        }
        if (desc.isGenericDescriptor() || desc.isDataDescriptor()) {
            putDirect(propertyName, desc.value() ? desc.value() : jsUndefined(), desc.attributes());
        } else if (desc.isAccessorDescriptor()) {
            GetterSetterImp *gs = new GetterSetterImp();
            putDirect(propertyName, gs, desc.attributes() | GetterSetter);
            _prop.setHasGetterSetterProperties(true);
            if (desc.getter() && !desc.getter()->isUndefined())
                gs->setGetter(desc.getter()->toObject(exec));
            if (desc.setter() && !desc.setter()->isUndefined())
                gs->setSetter(desc.setter()->toObject(exec));
        }
        return true;
    }

    //Step 5
    if (desc.isEmpty())
        return true;

    //Step 6
    if (desc.equalTo(exec, current))
        return true;

    //Step 7
    // Filter out invalid unconfigurable configurations
    if (!current.configurable()) {
        if (desc.configurable()) {
            if (shouldThrow)
                throwError(exec, TypeError, "can not redefine non-configurable property \'" + propertyName.ustring() + "\'");
            return false;
        }
        if (desc.enumerableSet() && desc.enumerable() != current.enumerable()) {
            if (shouldThrow)
                throwError(exec, TypeError, "can not change enumerable attribute of unconfigurable property \'" + propertyName.ustring() + "\'");
            return false;
        }
    }

    //Step 8.
    if (!desc.isGenericDescriptor()) {
        if (current.isDataDescriptor() != desc.isDataDescriptor()) { // Step 9
            // DataDescriptor updating to AccessorDescriptor, or the other way.
            if (!current.configurable()) {
                if (shouldThrow)
                    throwError(exec, TypeError, "can not change access mechanism for an unconfigurable property \'" + propertyName.ustring() + "\'");
                return false;
            }

            deleteProperty(exec, propertyName);

            if (current.isDataDescriptor()) {
                // Updating from DataDescriptor to AccessorDescriptor
                GetterSetterImp *gs = new GetterSetterImp();
                putDirect(propertyName, gs, current.attributesWithOverride(desc) | GetterSetter);
                _prop.setHasGetterSetterProperties(true);

                if (desc.getter()) {
                    if (desc.getter()->isUndefined())
                        gs->setGetter(0);
                    else
                        gs->setGetter(desc.getter()->toObject(exec));
                }
                if (desc.setter()) {
                    if (desc.setter()->isUndefined())
                        gs->setSetter(0);
                    else
                        gs->setSetter(desc.setter()->toObject(exec));
                }
            } else {
                // Updating from AccessorDescriptor to DataDescriptor
                unsigned int newAttr = current.attributesWithOverride(desc);
                if (!desc.writable())
                    newAttr |= ReadOnly;
                putDirect(propertyName, desc.value() ? desc.value() : jsUndefined(), newAttr);
            }
            return true;
        } else if (current.isDataDescriptor() && desc.isDataDescriptor()) { //Step 10
            // Just updating the value here
            if (!current.configurable()) {
                if (!current.writable() && desc.writable()) {
                    if (shouldThrow)
                        throwError(exec, TypeError, "can not change writable attribute of unconfigurable property \'" + propertyName.ustring() + "\'");
                    return false;
                }
                if (!current.writable()) {
                    if (desc.value() && !(current.value() && sameValue(exec, current.value(), desc.value()))) {
                        if (shouldThrow)
                            throwError(exec, TypeError, "can not change value of a readonly property \'" + propertyName.ustring() + "\'");
                        return false;
                    }
                }
            } else {
                if (!deleteProperty(exec, propertyName))
                    removeDirect(propertyName);

                putDirect(propertyName, desc.value() ? desc.value() : current.value(), current.attributesWithOverride(desc));
                return true;
            }
        } else if (current.isAccessorDescriptor() && desc.isAccessorDescriptor()) { // Step 11
            // Filter out unconfigurable combinations
            if (!current.configurable()) {
                if (desc.setter() && !sameValue(exec, desc.setter(), current.setter() ? current.setter() : jsUndefined())) {
                    if (shouldThrow)
                        throwError(exec, TypeError, "can not change the setter of an unconfigurable property \'" + propertyName.ustring() + "\'");
                    return false;
                }
                if (desc.getter() && !sameValue(exec, desc.getter(), current.getter() ? current.getter() : jsUndefined())) {
                    if (shouldThrow)
                        throwError(exec, TypeError, "can not change the getter of an unconfigurable property \'" + propertyName.ustring() + "\'");
                    return false;
                }
            }
        }
    }

    //Step 12
    // Everything is allowed here, updating GetterSetter, storing new value
    JSValue* jsval = getDirect(propertyName);
    unsigned int newAttr = current.attributesWithOverride(desc);
    if (jsval && jsval->type() == GetterSetterType) {
        GetterSetterImp *gs = static_cast<GetterSetterImp*>(jsval);
        if (desc.getter()) {
            if (desc.getter()->isUndefined())
                gs->setGetter(0);
            else
                gs->setGetter(desc.getter()->toObject(exec));
        }
        if (desc.setter()) {
            if (desc.setter()->isUndefined())
                gs->setSetter(0);
            else
                gs->setSetter(desc.setter()->toObject(exec));
        }
    } else
        jsval = desc.value() ? desc.value() : current.value();

    deleteProperty(exec, propertyName);
    if (jsval->type() == GetterSetterType) {
        putDirect(propertyName, jsval, newAttr | GetterSetter);
        _prop.setHasGetterSetterProperties(true);
    } else
        put(exec, propertyName, jsval, newAttr);

    return true; //Step 13
}

void JSObject::preventExtensions()
{
    if (isExtensible())
        _prop.setExtensible(false);
}

bool JSObject::implementsConstruct() const
{
  return false;
}

JSObject* JSObject::construct(ExecState*, const List& /*args*/)
{
  assert(false);
  return NULL;
}

JSObject* JSObject::construct(ExecState* exec, const List& args, const Identifier& /*functionName*/, const UString& /*sourceURL*/, int /*lineNumber*/)
{
  return construct(exec, args);
}

JSObject* JSObject::valueClone(Interpreter* /*targetCtx*/) const
{
    return 0;
}

bool JSObject::isFunctionType() const
{
  return implementsCall();
}

JSValue *JSObject::callAsFunction(ExecState * /*exec*/, JSObject * /*thisObj*/, const List &/*args*/)
{
  assert(false);
  return NULL;
}

bool JSObject::implementsHasInstance() const
{
  return false;
}

bool JSObject::hasInstance(ExecState* exec, JSValue* value)
{
    JSValue* proto = get(exec, exec->propertyNames().prototype);
    if (!proto->isObject()) {
        throwError(exec, TypeError, "instanceof called on an object with an invalid prototype property.");
        return false;
    }

    if (!value->isObject())
        return false;

    JSObject* o = static_cast<JSObject*>(value);
    while ((o = o->prototype()->getObject())) {
        if (o == proto)
            return true;
    }
    return false;
}

bool JSObject::propertyIsEnumerable(ExecState*, const Identifier& propertyName) const
{
  unsigned attributes;

  if (!getPropertyAttributes(propertyName, attributes))
    return false;
  else
    return !(attributes & DontEnum);
}

bool JSObject::getPropertyAttributes(const Identifier& propertyName, unsigned& attributes) const
{
  if (_prop.get(propertyName, attributes))
    return true;

  // Look in the static hashtable of properties
  const HashEntry* e = findPropertyHashEntry(propertyName);
  if (e) {
    attributes = e->attr;
    return true;
  }

  return false;
}

void JSObject::getOwnPropertyNames(ExecState* /*exec*/, PropertyNameArray& propertyNames, PropertyMap::PropertyMode mode)
{
  _prop.getPropertyNames(propertyNames, mode);

  // Add properties from the static hashtable of properties
  const ClassInfo *info = classInfo();
  while (info) {
    if (info->propHashTable) {
      int size = info->propHashTable->size;
      const HashEntry *e = info->propHashTable->entries;
      for (int i = 0; i < size; ++i, ++e) {
        if (e->s && PropertyMap::checkEnumerable(e->attr, mode))
          propertyNames.add(e->s);
      }
    }
    info = info->parentClass;
  }
}

bool JSObject::toBoolean(ExecState * /*exec*/) const
{
  return true;
}

double JSObject::toNumber(ExecState *exec) const
{
  JSValue *prim = toPrimitive(exec,NumberType);
  if (exec->hadException()) // should be picked up soon in nodes.cpp
    return 0.0;
  return prim->toNumber(exec);
}

UString JSObject::toString(ExecState *exec) const
{
  JSValue *prim = toPrimitive(exec,StringType);
  if (exec->hadException()) // should be picked up soon in nodes.cpp
    return UString(UString::empty);
  return prim->toString(exec);
}

JSObject *JSObject::toObject(ExecState * /*exec*/) const
{
  return const_cast<JSObject*>(this);
}

void JSObject::putDirect(const Identifier &propertyName, int value, int attr)
{
    _prop.put(propertyName, jsNumber(value), attr);
}

void JSObject::removeDirect(const Identifier &propertyName)
{
    _prop.remove(propertyName);
}

void JSObject::putDirectFunction(InternalFunctionImp* func, int attr)
{
    putDirect(func->functionName(), func, attr);
}

void JSObject::fillGetterPropertySlot(PropertySlot& slot, JSValue **location)
{
    GetterSetterImp *gs = static_cast<GetterSetterImp *>(*location);
    JSObject *getterFunc = gs->getGetter();
    if (getterFunc)
        slot.setGetterSlot(this, getterFunc);
    else
        slot.setUndefined(this);
}

// ------------------------------ Error ----------------------------------------

const char * const errorNamesArr[] = {
  I18N_NOOP("Error"), // GeneralError
  I18N_NOOP("Evaluation error"), // EvalError
  I18N_NOOP("Range error"), // RangeError
  I18N_NOOP("Reference error"), // ReferenceError
  I18N_NOOP("Syntax error"), // SyntaxError
  I18N_NOOP("Type error"), // TypeError
  I18N_NOOP("URI error"), // URIError
};

const char * const * const Error::errorNames = errorNamesArr;

JSObject *Error::create(ExecState *exec, ErrorType errtype, const UString &message,
                         int lineno, int sourceId, const UString &sourceURL)
{
#ifdef KJS_VERBOSE
  // message could be 0L. Don't enable this on Solaris ;)
  fprintf(stderr, "WARNING: KJS %s: %s\n", errorNamesArr[errtype], message.ascii());
#endif


  Interpreter* interp = exec->lexicalInterpreter();
  JSObject *cons;
  switch (errtype) {
  case EvalError:
    cons = interp->builtinEvalError();
    break;
  case RangeError:
    cons = interp->builtinRangeError();
    break;
  case ReferenceError:
    cons = interp->builtinReferenceError();
    break;
  case SyntaxError:
    cons = interp->builtinSyntaxError();
    break;
  case TypeError:
    cons = interp->builtinTypeError();
    break;
  case URIError:
    cons = interp->builtinURIError();
    break;
  default:
    cons = interp->builtinError();
    break;
  }

  List args;
  if (message.isEmpty())
    args.append(jsString(errorNames[errtype]));
  else
    args.append(jsString(message));
  JSObject *err = static_cast<JSObject *>(cons->construct(exec,args));

  if (lineno != -1)
    err->put(exec, "line", jsNumber(lineno));
  if (sourceId != -1)
    err->put(exec, "sourceId", jsNumber(sourceId));

  if(!sourceURL.isNull())
    err->put(exec, "sourceURL", jsString(sourceURL));

  return err;

/*
#ifndef NDEBUG
  const char *msg = err->get(messagePropertyName)->toString().value().ascii();
  if (l >= 0)
      fprintf(stderr, "KJS: %s at line %d. %s\n", estr, l, msg);
  else
      fprintf(stderr, "KJS: %s. %s\n", estr, msg);
#endif

  return err;
*/
}

JSObject *Error::create(ExecState *exec, ErrorType type, const char *message)
{
    return create(exec, type, message, -1, -1, NULL);
}

JSObject *throwError(ExecState *exec, ErrorType type)
{
    JSObject *error = Error::create(exec, type, UString(), -1, -1, NULL);
    exec->setException(error);
    return error;
}

JSObject *throwError(ExecState *exec, ErrorType type, const UString &message)
{
    JSObject *error = Error::create(exec, type, message, -1, -1, NULL);
    exec->setException(error);
    return error;
}

JSObject *throwError(ExecState *exec, ErrorType type, const char *message)
{
    JSObject *error = Error::create(exec, type, message, -1, -1, NULL);
    exec->setException(error);
    return error;
}

JSObject *throwError(ExecState *exec, ErrorType type, const UString &message, int line, int sourceId, const UString &sourceURL)
{
    JSObject *error = Error::create(exec, type, message, line, sourceId, sourceURL);
    exec->setException(error);
    return error;
}

} // namespace KJS
