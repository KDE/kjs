/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2003 Apple Computer, Inc.
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

#include "error_object.h"

#include "value.h"
#include "object.h"
#include "types.h"
#include "interpreter.h"
#include "operations.h"
//#include "debugger.h"

using namespace KJS;

// ------------------------------ ErrorInstance ----------------------------

const ClassInfo ErrorInstance::info = {"Error", nullptr, nullptr, nullptr};

ErrorInstance::ErrorInstance(JSObject *proto)
    : JSObject(proto)
{
}

// ------------------------------ ErrorPrototype ----------------------------

// ECMA 15.9.4
ErrorPrototype::ErrorPrototype(ExecState *exec,
                               ObjectPrototype *objectProto,
                               FunctionPrototype *funcProto)
    : ErrorInstance(objectProto)
{
    // Interpreter::initGlobalObject sets the constructor property
    // on the prototypes for this and the native error types

    put(exec, exec->propertyNames().name,     jsString("Error"), DontEnum);
    // ECMA Edition 5.1r6 - 15.11.4.3
    // The initial value of Error.prototype.message is the empty String.
    put(exec, exec->propertyNames().message,  jsString(""), DontEnum);
    putDirectFunction(new ErrorProtoFunc(exec, funcProto, exec->propertyNames().toString), DontEnum);
}

// ------------------------------ ErrorProtoFunc ----------------------------

ErrorProtoFunc::ErrorProtoFunc(ExecState *exec, FunctionPrototype *funcProto, const Identifier &name)
    : InternalFunctionImp(funcProto, name)
{
    putDirect(exec->propertyNames().length, jsNumber(0), DontDelete | ReadOnly | DontEnum);
}

JSValue *ErrorProtoFunc::callAsFunction(ExecState *exec, JSObject *thisObj, const List &/*args*/)
{
    // toString()
    UString name;
    JSValue *v = thisObj->get(exec, exec->propertyNames().name);
    if (!v->isUndefined()) {
        name = v->toString(exec);
    } else {
        name = "Error";
    }

    UString message;
    v = thisObj->get(exec, exec->propertyNames().message);
    if (!v->isUndefined()) {
        message = v->toString(exec);
    }

    if (name.isEmpty()) {
        return jsString(message);
    }
    if (message.isEmpty()) {
        return jsString(name);
    }

    return jsString(name + ": " + message);
}

// ------------------------------ ErrorObjectImp -------------------------------

ErrorObjectImp::ErrorObjectImp(ExecState *exec, FunctionPrototype *funcProto, ErrorPrototype *errorProto)
    : InternalFunctionImp(funcProto)
{
    // ECMA 15.11.3.1 Error.prototype
    putDirect(exec->propertyNames().prototype, errorProto, DontEnum | DontDelete | ReadOnly);
    putDirect(exec->propertyNames().length, jsNumber(1), DontDelete | ReadOnly | DontEnum);
    //putDirect(namePropertyName, jsString(n));
}

bool ErrorObjectImp::implementsConstruct() const
{
    return true;
}

// ECMA 15.9.3
JSObject *ErrorObjectImp::construct(ExecState *exec, const List &args)
{
    JSObject *proto = static_cast<JSObject *>(exec->lexicalInterpreter()->builtinErrorPrototype());
    JSObject *imp = new ErrorInstance(proto);
    JSObject *obj(imp);

    if (!args[0]->isUndefined()) {
        imp->putDirect(exec->propertyNames().message, jsString(args[0]->toString(exec)));
    }

    return obj;
}

// ECMA 15.9.2
JSValue *ErrorObjectImp::callAsFunction(ExecState *exec, JSObject * /*thisObj*/, const List &args)
{
    // "Error()" gives the sames result as "new Error()"
    return construct(exec, args);
}

// ------------------------------ NativeErrorPrototype ----------------------

NativeErrorPrototype::NativeErrorPrototype(ExecState *exec, ErrorPrototype *errorProto, ErrorType et, UString name, UString message)
    : JSObject(errorProto)
{
    errType = et;
    putDirect(exec->propertyNames().name, jsString(name), 0);
    putDirect(exec->propertyNames().message, jsString(message), 0);
}

// ------------------------------ NativeErrorImp -------------------------------

const ClassInfo NativeErrorImp::info = {"Function", &InternalFunctionImp::info, nullptr, nullptr};

NativeErrorImp::NativeErrorImp(ExecState *exec, FunctionPrototype *funcProto, JSObject *prot)
    : InternalFunctionImp(funcProto)
    , proto(prot)
{
    putDirect(exec->propertyNames().length, jsNumber(1), DontDelete | ReadOnly | DontEnum); // ECMA 15.11.7.5
    putDirect(exec->propertyNames().prototype, proto, DontDelete | ReadOnly | DontEnum);
}

bool NativeErrorImp::implementsConstruct() const
{
    return true;
}

JSObject *NativeErrorImp::construct(ExecState *exec, const List &args)
{
    JSObject *imp = new ErrorInstance(proto);
    JSObject *obj(imp);
    if (!args[0]->isUndefined()) {
        imp->putDirect(exec->propertyNames().message, jsString(args[0]->toString(exec)));
    }
    return obj;
}

JSValue *NativeErrorImp::callAsFunction(ExecState *exec, JSObject *, const List &args)
{
    return construct(exec, args);
}

void NativeErrorImp::mark()
{
    InternalFunctionImp::mark();
    if (proto && !proto->marked()) {
        proto->mark();
    }
}
