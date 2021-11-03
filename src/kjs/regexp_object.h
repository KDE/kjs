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

#ifndef _REGEXP_OBJECT_H_
#define _REGEXP_OBJECT_H_

#include "internal.h"
#include "function_object.h"
#include "regexp.h"
#include <wtf/OwnArrayPtr.h>

namespace KJS
{
class ExecState;
class RegExpPrototype : public JSObject
{
public:
    RegExpPrototype(ExecState *exec,
                    ObjectPrototype *objProto,
                    FunctionPrototype *funcProto);
    const ClassInfo *classInfo() const override
    {
        return &info;
    }
    static const ClassInfo info;
};

class RegExpProtoFunc : public InternalFunctionImp
{
public:
    RegExpProtoFunc(ExecState *, FunctionPrototype *, int i, int len, const Identifier &);

    JSValue *callAsFunction(ExecState *exec, JSObject *thisObj, const List &args) override;

    enum { Exec, Test, ToString, Compile };
private:
    int id;
};

class RegExpImp : public JSObject
{
public:
    RegExpImp(RegExpPrototype *regexpProto);
    ~RegExpImp() override;
    void setRegExp(ExecState *exec, RegExp *r);
    RegExp *regExp() const
    {
        return reg;
    }

    const ClassInfo *classInfo() const override
    {
        return &info;
    }
    static const ClassInfo info;

    JSObject *valueClone(Interpreter *targetCtx) const override;
private:
    RegExp *reg;
};

struct RegExpObjectImpPrivate;

class RegExpObjectImp : public InternalFunctionImp
{
public:
    enum { Dollar1, Dollar2, Dollar3, Dollar4, Dollar5, Dollar6, Dollar7, Dollar8, Dollar9,
           Input, Multiline, LastMatch, LastParen, LeftContext, RightContext
         };

    RegExpObjectImp(ExecState *exec,
                    FunctionPrototype *funcProto,
                    RegExpPrototype *regProto);
    bool implementsConstruct() const override;
    using KJS::JSObject::construct;
    JSObject *construct(ExecState *exec, const List &args) override;
    JSValue *callAsFunction(ExecState *exec, JSObject *thisObj, const List &args) override;

    using KJS::JSObject::put;
    void put(ExecState *, const Identifier &, JSValue *, int attr = None) override;
    void putValueProperty(ExecState *, int token, JSValue *, int attr);
    using KJS::JSObject::getOwnPropertySlot;
    bool getOwnPropertySlot(ExecState *, const Identifier &, PropertySlot &) override;
    JSValue *getValueProperty(ExecState *, int token) const;

    // If resources are exhausted during a match, exec parameter will have an exception
    // set, and endOffset will be -1
    UString performMatch(RegExp *, ExecState *, const RegExpStringContext &, const UString &,
                         int startOffset = 0, int *endOffset = nullptr, int **ovector = nullptr);
    JSObject *arrayOfMatches(ExecState *exec, const UString &result) const;

    static void throwRegExpError(ExecState *);

    const ClassInfo *classInfo() const override
    {
        return &info;
    }

    /*
     Attempts to create a new regular expression engine for the string p
     and the flags stored in flagsInput. If this succeeds, it returns the
     engine. If not, it returns 0, and raises an exception in exec
    */
    static RegExp *makeEngine(ExecState *exec, const UString &p, JSValue *flagsInput);
private:
    JSValue *getBackref(int) const;
    JSValue *getLastMatch() const;
    JSValue *getLastParen() const;
    JSValue *getLeftContext() const;
    JSValue *getRightContext() const;

    OwnPtr<RegExpObjectImpPrivate> d;

    static const ClassInfo info;
};

} // namespace

#endif
