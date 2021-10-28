/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003, 2004, 2005, 2006 Apple Computer, Inc.
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

#ifndef INTERNAL_H
#define INTERNAL_H

#include "JSType.h"
#include "interpreter.h"
#include "object.h"
#include "protect.h"
#include "scope_chain.h"
#include "types.h"
#include "ustring.h"

#include <wtf/Noncopyable.h>

#ifndef I18N_NOOP
#define I18N_NOOP(s) s
#endif

namespace KJS
{

// ---------------------------------------------------------------------------
//                            Primitive impls
// ---------------------------------------------------------------------------

class StringImp : public JSCell
{
public:
    StringImp() : val(UString::empty) { }
    StringImp(const UString &v) : val(v)
    {
        Collector::reportExtraMemoryCost(v.cost());
    }
    enum HasOtherOwnerType { HasOtherOwner }; // e.g. storage cost already accounted for
    StringImp(const UString &value, HasOtherOwnerType) : val(value) { }
    StringImp(const char *v) : val(v) { }
    StringImp(const char *v, int len) : val(v, len) { }
    const UString &value() const
    {
        return val;
    }

    JSType type() const override
    {
        return StringType;
    }

    JSValue *toPrimitive(ExecState *exec, JSType preferred = UnspecifiedType) const override;
    bool getPrimitiveNumber(ExecState *, double &number, JSValue *&value) override;
    bool toBoolean(ExecState *exec) const override;
    double toNumber(ExecState *exec) const override;
    UString toString(ExecState *exec) const override;
    JSObject *toObject(ExecState *exec) const override;

private:
    UString val;
};

class NumberImp : public JSCell
{
    friend class ConstantValues;
    friend KJS_EXPORT JSValue *jsNumberCell(double);
public:
    double value() const
    {
        return val;
    }

    JSType type() const override
    {
        return NumberType;
    }

    JSValue *toPrimitive(ExecState *exec, JSType preferred = UnspecifiedType) const override;
    bool getPrimitiveNumber(ExecState *, double &number, JSValue *&value) override;
    bool toBoolean(ExecState *exec) const override;
    double toNumber(ExecState *exec) const override;
    UString toString(ExecState *exec) const override;
    JSObject *toObject(ExecState *exec) const override;

private:
    NumberImp(double v) : val(v) { }

    bool getUInt32(uint32_t &) const override;
    bool getTruncatedInt32(int32_t &) const override;
    bool getTruncatedUInt32(uint32_t &) const override;

    double val;
};

// ---------------------------------------------------------------------------
//                            Evaluation
// ---------------------------------------------------------------------------

struct AttachedInterpreter;
class DebuggerImp
{
public:

    DebuggerImp()
    {
        interps = nullptr;
        isAborted = false;
    }

    void abort()
    {
        isAborted = true;
    }
    bool aborted() const
    {
        return isAborted;
    }

    AttachedInterpreter *interps;
    bool isAborted;
};

// helper function for toInteger, toInt32, toUInt32 and toUInt16
double roundValue(double d);
inline double roundValue(ExecState *e, JSValue *v)
{
    return roundValue(JSValue::toNumber(v, e));
}

int32_t toInt32(double dd);
uint32_t toUInt32(double dd);
uint16_t toUInt16(double dd);

//#ifndef NDEBUG
void printInfo(ExecState *exec, const char *s, JSValue *, int lineno = -1);
//#endif

} // namespace

#endif //  INTERNAL_H
