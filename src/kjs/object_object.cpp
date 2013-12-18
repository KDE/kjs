// -*- c-basic-offset: 2 -*-
// krazy:excludeall=doublequote_chars (UStrings aren't QStrings)
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
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

#include "object_object.h"

#include "operations.h"
#include "function_object.h"
#include "propertydescriptor.h"
#include <stdio.h>

namespace KJS {


/**
 * @internal
 *
 * Class to implement all methods that are properties of the
 * Object object
 */
class ObjectObjectFuncImp : public InternalFunctionImp {
public:
    ObjectObjectFuncImp(ExecState *, FunctionPrototype *, int i, int len, const Identifier& );

    virtual JSValue *callAsFunction(ExecState *, JSObject *thisObj, const List &args);

    enum { GetOwnPropertyDescriptor, DefineProperty, GetPrototypeOf,
           GetOwnPropertyNames, Keys, DefineProperties, Create, IsExtensible,
           PreventExtensible, IsSealed, Seal, IsFrozen, Freeze };

private:
    int id;
};

// ------------------------------ ObjectPrototype --------------------------------

ObjectPrototype::ObjectPrototype(ExecState* exec, FunctionPrototype* funcProto)
  : JSObject() // [[Prototype]] is null
{
    static const Identifier* hasOwnPropertyPropertyName = new Identifier("hasOwnProperty");
    static const Identifier* propertyIsEnumerablePropertyName = new Identifier("propertyIsEnumerable");
    static const Identifier* isPrototypeOfPropertyName = new Identifier("isPrototypeOf");
    static const Identifier* defineGetterPropertyName = new Identifier("__defineGetter__");
    static const Identifier* defineSetterPropertyName = new Identifier("__defineSetter__");
    static const Identifier* lookupGetterPropertyName = new Identifier("__lookupGetter__");
    static const Identifier* lookupSetterPropertyName = new Identifier("__lookupSetter__");

    putDirectFunction(new ObjectProtoFunc(exec, funcProto, ObjectProtoFunc::ToString, 0, exec->propertyNames().toString), DontEnum);
    putDirectFunction(new ObjectProtoFunc(exec, funcProto, ObjectProtoFunc::ToLocaleString, 0, exec->propertyNames().toLocaleString), DontEnum);
    putDirectFunction(new ObjectProtoFunc(exec, funcProto, ObjectProtoFunc::ValueOf, 0, exec->propertyNames().valueOf), DontEnum);
    putDirectFunction(new ObjectProtoFunc(exec, funcProto, ObjectProtoFunc::HasOwnProperty, 1, *hasOwnPropertyPropertyName), DontEnum);
    putDirectFunction(new ObjectProtoFunc(exec, funcProto, ObjectProtoFunc::PropertyIsEnumerable, 1, *propertyIsEnumerablePropertyName), DontEnum);
    putDirectFunction(new ObjectProtoFunc(exec, funcProto, ObjectProtoFunc::IsPrototypeOf, 1, *isPrototypeOfPropertyName), DontEnum);

    // Mozilla extensions
    putDirectFunction(new ObjectProtoFunc(exec, funcProto, ObjectProtoFunc::DefineGetter, 2, *defineGetterPropertyName), DontEnum);
    putDirectFunction(new ObjectProtoFunc(exec, funcProto, ObjectProtoFunc::DefineSetter, 2, *defineSetterPropertyName), DontEnum);
    putDirectFunction(new ObjectProtoFunc(exec, funcProto, ObjectProtoFunc::LookupGetter, 1, *lookupGetterPropertyName), DontEnum);
    putDirectFunction(new ObjectProtoFunc(exec, funcProto, ObjectProtoFunc::LookupSetter, 1, *lookupSetterPropertyName), DontEnum);
}

JSObject* ObjectPrototype::self(ExecState* exec)
{
    return exec->lexicalInterpreter()->builtinObjectPrototype();
}

// ------------------------------ ObjectProtoFunc --------------------------------

ObjectProtoFunc::ObjectProtoFunc(ExecState* exec, FunctionPrototype* funcProto, int i, int len, const Identifier& name)
  : InternalFunctionImp(funcProto, name)
  , id(i)
{
  putDirect(exec->propertyNames().length, len, DontDelete|ReadOnly|DontEnum);
}


// ECMA 15.2.4.2, 15.2.4.4, 15.2.4.5, 15.2.4.7

JSValue *ObjectProtoFunc::callAsFunction(ExecState *exec, JSObject *thisObj, const List &args)
{
    switch (id) {
        case ValueOf:
            return thisObj;
        case HasOwnProperty: {
            PropertySlot slot;
            return jsBoolean(thisObj->getOwnPropertySlot(exec, Identifier(args[0]->toString(exec)), slot));
        }
        case IsPrototypeOf: {
            if (!args[0]->isObject())
                return jsBoolean(false);
         
            JSValue *v = static_cast<JSObject *>(args[0])->prototype();

            while (true) {
                if (!v->isObject())
                    return jsBoolean(false);
                
                if (thisObj == static_cast<JSObject *>(v))
                    return jsBoolean(true);
                
                v = static_cast<JSObject *>(v)->prototype();
            }
        }
        case DefineGetter: 
        case DefineSetter: {
            if (!args[1]->isObject() ||
                !static_cast<JSObject *>(args[1])->implementsCall()) {
                if (id == DefineGetter)
                    return throwError(exec, SyntaxError, "invalid getter usage");
                else
                    return throwError(exec, SyntaxError, "invalid setter usage");
            }

            if (id == DefineGetter)
                thisObj->defineGetter(exec, Identifier(args[0]->toString(exec)), static_cast<JSObject *>(args[1]));
            else
                thisObj->defineSetter(exec, Identifier(args[0]->toString(exec)), static_cast<JSObject *>(args[1]));
            return jsUndefined();
        }
        case LookupGetter:
        case LookupSetter: {
            Identifier propertyName = Identifier(args[0]->toString(exec));
            
            JSObject *obj = thisObj;
            while (true) {
                JSValue *v = obj->getDirect(propertyName);
                
                if (v) {
                    if (v->type() != GetterSetterType)
                        return jsUndefined();

                    JSObject *funcObj;
                        
                    if (id == LookupGetter)
                        funcObj = static_cast<GetterSetterImp *>(v)->getGetter();
                    else
                        funcObj = static_cast<GetterSetterImp *>(v)->getSetter();
                
                    if (!funcObj)
                        return jsUndefined();
                    else
                        return funcObj;
                }
                
                if (!obj->prototype() || !obj->prototype()->isObject())
                    return jsUndefined();
                
                obj = static_cast<JSObject *>(obj->prototype());
            }
        }
        case PropertyIsEnumerable:
            return jsBoolean(thisObj->propertyIsEnumerable(exec, Identifier(args[0]->toString(exec))));
        case ToLocaleString:
            return jsString(thisObj->toString(exec));
        case ToString:
        default:
            return jsString("[object " + thisObj->className() + "]");
    }
}

// ------------------------------ ObjectObjectImp --------------------------------

ObjectObjectImp::ObjectObjectImp(ExecState* exec, ObjectPrototype* objProto, FunctionPrototype* funcProto)
  : InternalFunctionImp(funcProto)
{
  static const Identifier* getOwnPropertyDescriptorName = new Identifier("getOwnPropertyDescriptor");
  static const Identifier* createName = new Identifier("create");
  static const Identifier* definePropertyName = new Identifier("defineProperty");
  static const Identifier* definePropertiesName = new Identifier("defineProperties");
  static const Identifier* getPrototypeOf = new Identifier("getPrototypeOf");
  static const Identifier* getOwnPropertyNames = new Identifier("getOwnPropertyNames");
  static const Identifier* sealName = new Identifier("seal");
  static const Identifier* freezeName = new Identifier("freeze");
  static const Identifier* preventExtensionsName = new Identifier("preventExtensions");
  static const Identifier* isSealedName = new Identifier("isSealed");
  static const Identifier* isFrozenName = new Identifier("isFrozen");
  static const Identifier* isExtensibleName = new Identifier("isExtensible");
  static const Identifier* keys = new Identifier("keys");

  // ECMA 15.2.3.1
  putDirect(exec->propertyNames().prototype, objProto, DontEnum|DontDelete|ReadOnly);

  putDirectFunction(new ObjectObjectFuncImp(exec, funcProto, ObjectObjectFuncImp::GetOwnPropertyDescriptor, 2, *getOwnPropertyDescriptorName), DontEnum);
  putDirectFunction(new ObjectObjectFuncImp(exec, funcProto, ObjectObjectFuncImp::Create, 2, *createName), DontEnum);
  putDirectFunction(new ObjectObjectFuncImp(exec, funcProto, ObjectObjectFuncImp::DefineProperty, 3, *definePropertyName), DontEnum);
  putDirectFunction(new ObjectObjectFuncImp(exec, funcProto, ObjectObjectFuncImp::DefineProperties, 2, *definePropertiesName), DontEnum);
  putDirectFunction(new ObjectObjectFuncImp(exec, funcProto, ObjectObjectFuncImp::GetPrototypeOf, 1, *getPrototypeOf), DontEnum);
  putDirectFunction(new ObjectObjectFuncImp(exec, funcProto, ObjectObjectFuncImp::GetOwnPropertyNames, 1, *getOwnPropertyNames), DontEnum);
  putDirectFunction(new ObjectObjectFuncImp(exec, funcProto, ObjectObjectFuncImp::Seal, 1, *sealName), DontEnum);
  putDirectFunction(new ObjectObjectFuncImp(exec, funcProto, ObjectObjectFuncImp::Freeze, 1, *freezeName), DontEnum);
  putDirectFunction(new ObjectObjectFuncImp(exec, funcProto, ObjectObjectFuncImp::PreventExtensible, 1, *preventExtensionsName), DontEnum);
  putDirectFunction(new ObjectObjectFuncImp(exec, funcProto, ObjectObjectFuncImp::IsSealed, 1, *isSealedName), DontEnum);
  putDirectFunction(new ObjectObjectFuncImp(exec, funcProto, ObjectObjectFuncImp::IsFrozen, 1, *isFrozenName), DontEnum);
  putDirectFunction(new ObjectObjectFuncImp(exec, funcProto, ObjectObjectFuncImp::IsExtensible, 1, *isExtensibleName), DontEnum);
  putDirectFunction(new ObjectObjectFuncImp(exec, funcProto, ObjectObjectFuncImp::Keys, 1, *keys), DontEnum);

  // no. of arguments for constructor
  putDirect(exec->propertyNames().length, jsNumber(1), ReadOnly|DontDelete|DontEnum);
}


bool ObjectObjectImp::implementsConstruct() const
{
  return true;
}

// ECMA 15.2.2
JSObject* ObjectObjectImp::construct(ExecState* exec, const List& args)
{
  JSValue* arg = args[0];
  switch (arg->type()) {
  case StringType:
  case BooleanType:
  case NumberType:
  case ObjectType:
      return arg->toObject(exec);
  case NullType:
  case UndefinedType:
      return new JSObject(exec->lexicalInterpreter()->builtinObjectPrototype());
  default:
      //### ASSERT_NOT_REACHED();
      return 0;
  }
}

JSValue* ObjectObjectImp::callAsFunction(ExecState* exec, JSObject* /*thisObj*/, const List &args)
{
    return construct(exec, args);
}

// ------------------------------ ObjectObjectFuncImp ----------------------------

ObjectObjectFuncImp::ObjectObjectFuncImp(ExecState* exec, FunctionPrototype* funcProto, int i, int len, const Identifier& name)
    : InternalFunctionImp(funcProto, name), id(i)
{
    putDirect(exec->propertyNames().length, len, DontDelete|ReadOnly|DontEnum);
}

static JSValue* defineProperties(ExecState* exec, JSObject* object, JSValue* properties)
{
    JSObject* props = properties->toObject(exec);
    if (exec->hadException())
        return object;
    PropertyNameArray names;
    props->getOwnPropertyNames(exec, names, PropertyMap::ExcludeDontEnumProperties);
    int size = names.size();
    Vector<PropertyDescriptor> descriptors;
    for (int i = 0; i < size; ++i) {
        PropertyDescriptor desc;
        if (!desc.setPropertyDescriptorFromObject(exec, props->get(exec, names[i])))
            return jsUndefined();
        descriptors.append(desc);
    }
    for (int i = 0; i < size; ++i) {
        object->defineOwnProperty(exec, names[i], descriptors[i], true);
        if (exec->hadException())
            return jsUndefined();
    }
    return object;
}

JSValue *ObjectObjectFuncImp::callAsFunction(ExecState* exec, JSObject*, const List& args)
{
    switch (id) {
    case GetPrototypeOf: { //ECMA Edition 5.1r6 - 15.2.3.2
        JSObject* jso = args[0]->getObject();
        if (!jso)
            return throwError(exec, TypeError, "Not an Object");
        return jso->prototype();
    }
    case GetOwnPropertyDescriptor: { //ECMA Edition 5.1r6 - 15.2.3.3
        JSObject* jso = args[0]->getObject();
        if (!jso)
            return throwError(exec, TypeError, "Not an Object");

        UString name = args[1]->toString(exec);
        PropertyDescriptor desc;
        if (!jso->getOwnPropertyDescriptor(exec, Identifier(name), desc))
            return jsUndefined();
        return desc.fromPropertyDescriptor(exec);
    }
    case GetOwnPropertyNames: //ECMA Edition 5.1r6 - 15.2.3.4
    case Keys: { //ECMA Edition 5.1r6 - 15.2.3.14
        JSObject* jso = args[0]->getObject();
        if (!jso)
            return throwError(exec, TypeError, "Not an Object");

        JSObject *ret = static_cast<JSObject *>(exec->lexicalInterpreter()->builtinArray()->construct(exec, List::empty()));
        PropertyNameArray propertyNames;

        if (id == Keys)
            jso->getOwnPropertyNames(exec, propertyNames, PropertyMap::ExcludeDontEnumProperties);
        else // id == GetOwnPropertyNames
            jso->getOwnPropertyNames(exec, propertyNames, PropertyMap::IncludeDontEnumProperties);
        PropertyNameArrayIterator propEnd = propertyNames.end();
        unsigned int n = 0;
        for (PropertyNameArrayIterator propIter = propertyNames.begin(); propIter != propEnd; ++propIter) {
            Identifier name = *propIter;
            ret->put(exec, n, jsString(name.ustring()), None);
            ++n;
        }
        ret->put(exec, exec->propertyNames().length, jsNumber(n), DontEnum | DontDelete);
        return ret;
    }
    case Create: { //ECMA Edition 5.1r6 - 15.2.3.5
        JSObject *proto = args[0]->getObject();
        if (!proto && !args[0]->isNull())
            return throwError(exec, TypeError, "Not an Object");

        JSObject *ret = static_cast<JSObject *>(exec->lexicalInterpreter()->builtinObject()->construct(exec, List::empty()));
        if (proto)
            ret->setPrototype(proto);
        else
            ret->setPrototype(jsNull());
        if (args.size() >= 2 && !args[1]->isUndefined())
            return defineProperties(exec, ret, args[1]);
        return ret;
    }
    case DefineProperty: { //ECMA Edition 5.1r6 - 15.2.3.6
        JSObject* jso = args[0]->getObject();
        if (!jso)
            return throwError(exec, TypeError, "Not an Object");

        UString name = args[1]->toString(exec);
        PropertyDescriptor desc;
        if (!desc.setPropertyDescriptorFromObject(exec, args[2]))
            return jsUndefined();
        if (!jso->defineOwnProperty(exec, Identifier(name), desc, true))
            return jsUndefined();
        return jso;
    }
    case DefineProperties: { //ECMA Edition 5.1r6 - 15.2.3.7
        if (!args[0]->isObject())
            return throwError(exec, TypeError, "Not an Object");

        JSObject* jso = args[0]->getObject();
        return defineProperties(exec, jso, args[1]);
    }
    case Seal: { //ECMA Edition 5.1r6 - 15.2.3.8
        JSObject* jso = args[0]->getObject();
        if (!jso)
            return throwError(exec, TypeError, "Not an Object");

        PropertyNameArray names;
        jso->getOwnPropertyNames(exec, names, PropertyMap::IncludeDontEnumProperties);
        int size = names.size();

        PropertyDescriptor desc;
        for (int i = 0; i < size; ++i) {
            jso->getOwnPropertyDescriptor(exec, names[i], desc);
            if (desc.configurable()) {
                desc.setConfigureable(false);
                if (!jso->defineOwnProperty(exec, names[i], desc, true))
                    return jsUndefined();
            }
        }
        jso->preventExtensions();
        return jso;
    }
    case Freeze: { //ECMA Edition 5.1r6 - 15.2.3.9
        JSObject* jso = args[0]->getObject();
        if (!jso)
            return throwError(exec, TypeError, "Not an Object");

        PropertyNameArray names;
        jso->getOwnPropertyNames(exec, names, PropertyMap::IncludeDontEnumProperties);
        int size = names.size();

        PropertyDescriptor desc;
        for (int i = 0; i < size; ++i) {
            jso->getOwnPropertyDescriptor(exec, names[i], desc);
            if (desc.isDataDescriptor())
                if (desc.writable())
                    desc.setWritable(false);
            if (desc.configurable())
                desc.setConfigureable(false);
            if (!jso->defineOwnProperty(exec, names[i], desc, true))
                return jsUndefined();
        }
        jso->preventExtensions();
        return jso;
    }
    case PreventExtensible: { //ECMA Edition 5.1r6 - 15.2.3.10
        JSObject* jso = args[0]->getObject();
        if (!jso)
            return throwError(exec, TypeError, "Not an Object");
        jso->preventExtensions();
        return jso;
    }
    case IsSealed: { //ECMA Edition 5.1r6 - 15.2.3.11
        JSObject* jso = args[0]->getObject();
        if (!jso)
            return throwError(exec, TypeError, "Not an Object");

        PropertyNameArray names;
        jso->getOwnPropertyNames(exec, names, PropertyMap::IncludeDontEnumProperties);
        int size = names.size();

        PropertyDescriptor desc;
        for (int i = 0; i < size; ++i) {
            jso->getOwnPropertyDescriptor(exec, names[i], desc);
            if (desc.configurable())
                return jsBoolean(false);
        }
        return jsBoolean(!jso->isExtensible());
    }
    case IsFrozen: { //ECMA Edition 5.1r6 - 15.2.3.12
        JSObject* jso = args[0]->getObject();
        if (!jso)
            return throwError(exec, TypeError, "Not an Object");

        PropertyNameArray names;
        jso->getOwnPropertyNames(exec, names, PropertyMap::IncludeDontEnumProperties);
        int size = names.size();

        PropertyDescriptor desc;
        for (int i = 0; i < size; ++i) {
            jso->getOwnPropertyDescriptor(exec, names[i], desc);
            if (desc.isDataDescriptor())
                if (desc.writable())
                    return jsBoolean(false);
            if (desc.configurable())
                return jsBoolean(false);
        }
        return jsBoolean(!jso->isExtensible());
    }
    case IsExtensible: { //ECMA Edition 5.1r6 - 15.2.3.13
        JSObject* jso = args[0]->getObject();
        if (!jso)
            return throwError(exec, TypeError, "Not an Object");
        return jsBoolean(jso->isExtensible());
    }
    default:
        return jsUndefined();
    }
}

}   // namespace KJS
