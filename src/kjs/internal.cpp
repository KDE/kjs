/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2002 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2004 Apple Computer, Inc.
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

#include "internal.h"

#include "array_object.h"
#include "bool_object.h"
#include "collector.h"
#include "date_object.h"
#include "debugger.h"
#include "error_object.h"
#include "function_object.h"
#include "lexer.h"
#include "math_object.h"
#include "nodes.h"
#include "number_object.h"
#include "object.h"
#include "object_object.h"
#include "operations.h"
#include "regexp_object.h"
#include "string_object.h"
#include <assert.h>
#include <wtf/HashMap.h>
#include <wtf/HashSet.h>
#include <wtf/Vector.h>
#include <math.h>
#include <stdio.h>

namespace KJS
{

#if COMPILER(MSVC)
#define copysign _copysign
#endif

static const double D16 = 65536.0;
static const double D32 = 4294967296.0;

// ------------------------------ StringImp ------------------------------------

JSValue *StringImp::toPrimitive(ExecState *, JSType) const
{
    return const_cast<StringImp *>(this);
}

bool GetterSetterImp::getPrimitiveNumber(ExecState *, double &number, JSValue *&value)
{
    ASSERT_NOT_REACHED();
    number = 0;
    value = nullptr;
    return true;
}

bool StringImp::getPrimitiveNumber(ExecState *, double &number, JSValue *&value)
{
    value = this;
    number = val.toDouble();
    return false;
}

bool StringImp::toBoolean(ExecState *) const
{
    return (val.size() > 0);
}

double StringImp::toNumber(ExecState *) const
{
    return val.toDouble();
}

UString StringImp::toString(ExecState *) const
{
    return val;
}

JSObject *StringImp::toObject(ExecState *exec) const
{
    return new StringInstance(exec->lexicalInterpreter()->builtinStringPrototype(), const_cast<StringImp *>(this));
}

// ------------------------------ NumberImp ------------------------------------

JSValue *NumberImp::toPrimitive(ExecState *, JSType) const
{
    return const_cast<NumberImp *>(this);
}

bool NumberImp::getPrimitiveNumber(ExecState *, double &number, JSValue *&value)
{
    number = val;
    value = this;
    return true;
}

bool NumberImp::toBoolean(ExecState *) const
{
    return val < 0.0 || val > 0.0; // false for NaN
}

double NumberImp::toNumber(ExecState *) const
{
    return val;
}

UString NumberImp::toString(ExecState *) const
{
    if (val == 0.0) { // +0.0 or -0.0
        return "0";
    }
    return UString::from(val);
}

JSObject *NumberImp::toObject(ExecState *exec) const
{
    List args;
    args.append(const_cast<NumberImp *>(this));
    return static_cast<JSObject *>(exec->lexicalInterpreter()->builtinNumber()->construct(exec, args));
}

bool NumberImp::getUInt32(uint32_t &uint32) const
{
    uint32 = static_cast<uint32_t>(val);
    return uint32 == val;
}

bool NumberImp::getTruncatedInt32(int32_t &int32) const
{
    if (!(val >= -2147483648.0 && val < 2147483648.0)) {
        return false;
    }
    int32 = static_cast<int32_t>(val);
    return true;
}

bool NumberImp::getTruncatedUInt32(uint32_t &uint32) const
{
    if (!(val >= 0.0 && val < 4294967296.0)) {
        return false;
    }
    uint32 = static_cast<uint32_t>(val);
    return true;
}

// --------------------------- GetterSetterImp ---------------------------------
void GetterSetterImp::mark()
{
    JSCell::mark();

    if (getter && !getter->marked()) {
        getter->mark();
    }
    if (setter && !setter->marked()) {
        setter->mark();
    }
}

JSValue *GetterSetterImp::toPrimitive(ExecState *, JSType) const
{
    assert(false);
    return jsNull();
}

bool GetterSetterImp::toBoolean(ExecState *) const
{
    assert(false);
    return false;
}

double GetterSetterImp::toNumber(ExecState *) const
{
    assert(false);
    return 0.0;
}

UString GetterSetterImp::toString(ExecState *) const
{
    assert(false);
    return UString::null();
}

JSObject *GetterSetterImp::toObject(ExecState *exec) const
{
    assert(false);
    return jsNull()->toObject(exec);
}

// ------------------------------ InternalFunctionImp --------------------------

const ClassInfo InternalFunctionImp::info = {"Function", nullptr, nullptr, nullptr};

InternalFunctionImp::InternalFunctionImp()
{
}

InternalFunctionImp::InternalFunctionImp(FunctionPrototype *funcProto)
    : JSObject(funcProto)
{
}

InternalFunctionImp::InternalFunctionImp(FunctionPrototype *funcProto, const Identifier &name)
    : JSObject(funcProto)
    , m_name(name)
{
}

bool InternalFunctionImp::implementsCall() const
{
    return true;
}

bool InternalFunctionImp::implementsHasInstance() const
{
    return true;
}

// ------------------------------ global functions -----------------------------

double roundValue(double d)
{
    double ad = fabs(d);
    if (ad == 0 || isNaN(d) || isInf(d)) {
        return d;
    }
    return copysign(floor(ad), d);
}

int32_t toInt32(double d)
{
    if (isNaN(d) || isInf(d)) {
        return 0;
    }
    double d32 = fmod(roundValue(d), D32);

    if (d32 >= D32 / 2) {
        d32 -= D32;
    } else if (d32 < -D32 / 2) {
        d32 += D32;
    }

    return static_cast<int32_t>(d32);
}

int32_t toInt32(double d, bool &ok)
{
    ok = true;
    if (isNaN(d) || isInf(d)) {
        ok = false;
        return 0;
    }
    return toInt32(d);
}

uint32_t toUInt32(double dd)
{
    double d = roundValue(dd);
    if (isNaN(d) || isInf(d)) {
        return 0;
    }
    double d32 = fmod(d, D32);

    if (d32 < 0) {
        d32 += D32;
    }

    return static_cast<uint32_t>(d32);
}

uint16_t toUInt16(double dd)
{
    double d = roundValue(dd);
    if (isNaN(d) || isInf(d)) {
        return 0;
    }
    double d16 = fmod(d, D16);

    if (d16 < 0) {
        d16 += D16;
    }

    return static_cast<uint16_t>(d16);
}

//#ifndef NDEBUG
void printInfo(ExecState *exec, const char *s, JSValue *o, int lineno)
{
    UString vString;
    if (!o) {
        fprintf(stderr, "KJS: %s: (null)", s);
    } else {
        JSValue *v = o;

        unsigned int arrayLength = 0;
        bool hadExcep = exec->hadException();

        UString name;
        switch (v->type()) {
        case UnspecifiedType:
            name = "Unspecified";
            break;
        case UndefinedType:
            name = "Undefined";
            break;
        case NullType:
            name = "Null";
            break;
        case BooleanType:
            name = "Boolean";
            break;
        case StringType:
            name = "String";
            break;
        case NumberType:
            name = "Number";
            break;
        case ObjectType: {
            JSObject *obj = static_cast<JSObject *>(v);
            name = obj->className();
            if (name.isNull()) {
                name = "(unknown class)";
            }

            if (obj->inherits(&ArrayInstance::info)) {
                arrayLength = obj->get(exec, exec->propertyNames().length)->toUInt32(exec);
            }
            vString = "[object " + name + "]"; // krazy:exclude=doublequote_chars
            break;
        }
        case GetterSetterType:
            name = "GetterSetter";
            break;
        }

        // Avoid calling toString on a huge array (e.g. 4 billion elements, in mozilla/js/js1_5/Array/array-001.js)
        if (arrayLength > 100) {
            vString = UString("[ Array with ") + UString::from(arrayLength) + " elements ]";
        } else if (v->type() != ObjectType) { // Don't want to call a user toString function!
            vString = v->toString(exec);
        }
        if (!hadExcep) {
            exec->clearException();
        }

        if (vString.size() > 350) {
            vString = vString.substr(0, 350) + "...";
        }

        // Can't use two UString::ascii() in the same fprintf call
        CString tempString(vString.cstring());

        fprintf(stderr, "KJS: %s: %s : %s (%p)",
                s, tempString.c_str(), name.ascii(), (void *)v);

        if (lineno >= 0) {
            fprintf(stderr, ", line %d\n", lineno);
        } else {
            fprintf(stderr, "\n");
        }
    }
}
//#endif

}
