/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2012 Bernd Buschinski (b.buschinski@googlemail.com)
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

#include "json_object.h"

#include "jsonlexer.h"
#include "lookup.h"
#include "array_instance.h"
#include "jsonstringify.h"

#include "json_object.lut.h"

using namespace KJS;

// ------------------------------ JSONObjectImp --------------------------------

const ClassInfo JSONObjectImp::info = { "JSON", nullptr, &jsonTable, nullptr };

/* Source for json_object.lut.h
@begin jsonTable 2
  parse           JSONObjectImp::Parse      DontEnum|Function 2
  stringify       JSONObjectImp::Stringify  DontEnum|Function 3
@end
*/

JSONObjectImp::JSONObjectImp(ExecState *, ObjectPrototype *objProto)
    : JSObject(objProto)
{
}

bool JSONObjectImp::getOwnPropertySlot(ExecState *exec, const Identifier &propertyName, PropertySlot &slot)
{
    // As of ECMA 5.1r6 JSON only has 2 functions, so only functionSlot is needed
    return getStaticFunctionSlot<JSONFuncImp, JSObject>(exec, &jsonTable, this, propertyName, slot);
}

// ------------------------------ JSONFuncImp --------------------------------

JSONFuncImp::JSONFuncImp(ExecState *exec, int i, int l, const Identifier &name)
    : InternalFunctionImp(static_cast<FunctionPrototype *>(exec->lexicalInterpreter()->builtinFunctionPrototype()), name),
      id(i)
{
    putDirect(exec->propertyNames().length, l, DontDelete | ReadOnly | DontEnum);
}

static void reviver(ExecState *exec, JSValue *value, JSObject *func)
{
    if (exec->hadException()) {
        return;
    }

    JSType type = value->type();
    switch (type) {
    case ObjectType: {
        JSObject *obj = value->getObject();
        bool isArray = obj->inherits(&ArrayInstance::info);
        bool validArrayIndex = false;

        PropertyNameArray names;
        obj->getOwnPropertyNames(exec, names, KJS::PropertyMap::ExcludeDontEnumProperties);
        const int nameSize = names.size();
        for (int i = 0; i < nameSize; ++i) {
            // For Array only take properties that are valid Array indexes
            if (isArray) {
                names[i].toArrayIndex(&validArrayIndex);
                if (!validArrayIndex) {
                    continue;
                }
            }

            JSValue *val = obj->get(exec, names[i]);

            List args;
            args.append(jsString(names[i].ustring()));
            args.append(val);

            JSValue *ret = func->call(exec, obj, args);
            if (exec->hadException()) {
                return;
            }
            if (ret->isUndefined()) {
                obj->deleteProperty(exec, names[i]);
            } else {
                obj->put(exec, names[i], ret);
                reviver(exec, ret, func);
            }
        }
        break;
    }
    case NullType:
    case NumberType:
    case BooleanType:
    case StringType:
        break;
    case UnspecifiedType:
    case GetterSetterType:
    case UndefinedType:
        // should never be reached, as JSON doesn't know them
        // and we only have json data here
        ASSERT_NOT_REACHED();
        break;
    }
}

JSValue *JSONFuncImp::callAsFunction(ExecState *exec, JSObject * /*thisObj*/, const List &args)
{
    switch (id) {
    case JSONObjectImp::Parse: {
        if (args.size() < 1) {
            return throwError(exec, SyntaxError, "Invalid JSON Syntax");
        }

        JSONParser parser(args[0]->toString(exec));
        if (exec->hadException()) {
            return jsUndefined();
        }
        JSValue *val = parser.tryParse(exec);

        if (!val) {
            return throwError(exec, SyntaxError, "Invalid JSON Syntax");
        }

        if (args.size() < 2) {
            return val;
        }

        JSValue *func = args[1];
        if (func->implementsCall()) {
            JSObject *function = func->getObject();

            List args;
            args.append(jsString(""));
            args.append(val);

            JSObject *jsobjectArg = val->toObject(exec);
            if (exec->hadException()) {
                return jsUndefined();
            }
            JSValue *ret = function->call(exec, jsobjectArg, args);
            if (ret->isUndefined()) {
                return ret;
            } else {
                reviver(exec, ret, function);
                if (exec->hadException()) {
                    return jsUndefined();
                }
            }
        }

        return val;
    }
    case JSONObjectImp::Stringify: {
        JSValue *object = args[0];
        JSONStringify stringifier(exec, args[1], args[2]);

        JSONStringify::StringifyState state;
        JSValue *ret = stringifier.stringify(exec, object, state);
        switch (state) {
        case JSONStringify::Success:
            return ret;
        case JSONStringify::FailedCyclic:
            return throwError(exec, TypeError, "cyclic object value");
        case JSONStringify::FailedStackLimitExceeded:
            return throwError(exec, TypeError, "object stack limit exceeded");
        case JSONStringify::FailedException:
            //stringify already got an exception
            return jsUndefined();
        }
    }
    default:
        ASSERT_NOT_REACHED();
    }

    return jsUndefined();
}
