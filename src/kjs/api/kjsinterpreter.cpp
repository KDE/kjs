/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2008 Harri Porten (porten@kde.org)
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

#include "kjsinterpreter.h"
#include "kjsprivate.h"
#include "kjs/interpreter.h"
#include "kjs/completion.h"
#include "kjs/object.h"
#include "kjs/JSVariableObject.h"
#include <QString>
#include <stdio.h>

using namespace KJS;

class KJSResultHandle
{
public:
    KJSResultHandle() : rc(1), val(KJSUndefined()) { }

    int rc;
    KJSObject val;
    UString errMsg;

    void ref()
    {
        ++rc;
    }
    void deref()
    {
        if (--rc == 0) {
            delete this;
        }
    }
};

KJSResult::KJSResult()
    : hnd(new KJSResultHandle())
{
}

KJSResult::KJSResult(const KJSResult &r)
{
    hnd = r.hnd;
    hnd->ref();
}

KJSResult &KJSResult::operator=(const KJSResult &r)
{
    if (hnd != r.hnd) {
        r.hnd->ref();
        hnd->deref();
        hnd = r.hnd;
    }

    return *this;
}

KJSResult::~KJSResult()
{
    hnd->deref();
}

bool KJSResult::isException() const
{
    return !hnd->errMsg.isNull();
}

QString KJSResult::errorMessage() const
{
    return toQString(hnd->errMsg);
}

KJSObject KJSResult::value() const
{
    return hnd->val;
}

KJSInterpreter::KJSInterpreter()
    : globCtx(nullptr)
{
    Interpreter *ip = new Interpreter();
    ip->ref();
    hnd = INTERPRETER_HANDLE(ip);
}

KJSInterpreter::KJSInterpreter(const KJSGlobalObject &global)
    : globCtx(nullptr)
{
    JSValue *gv = JSVALUE(&global);
    assert(gv->isObject());
    JSObject *go = static_cast<JSObject *>(gv);
    assert(go->isGlobalObject());
    Interpreter *ip = new Interpreter(static_cast<JSGlobalObject *>(go));
    ip->ref();
    assert(go->prototype()->isObject());
    JSObject *p =  static_cast<JSObject *>(go->prototype());
    JSObject *objectProto = ip->builtinObjectPrototype();
    p->setPrototype(objectProto);
    hnd = INTERPRETER_HANDLE(ip);
}

KJSInterpreter::KJSInterpreter(const KJSInterpreter &other)
    : globCtx(nullptr)
{
    Interpreter *ip = INTERPRETER(&other);
    ip->ref();
    hnd = INTERPRETER_HANDLE(ip);
    globCtx.hnd = EXECSTATE_HANDLE(ip->globalExec());
}

KJSInterpreter &KJSInterpreter::operator=(const KJSInterpreter &other)
{
    Interpreter *thisIp = INTERPRETER(this);
    Interpreter *otherIp = INTERPRETER(&other);
    if (otherIp != thisIp) {
        otherIp->ref();
        thisIp->deref();
        hnd = INTERPRETER_HANDLE(otherIp);
        globCtx.hnd = EXECSTATE_HANDLE(otherIp->globalExec());
    }
    return *this;
}

KJSInterpreter::KJSInterpreter(KJSInterpreterHandle *h)
    : hnd(h), globCtx(nullptr)
{
    Interpreter *ip = INTERPRETER(this);
    globCtx.hnd = EXECSTATE_HANDLE(ip->globalExec());
}

KJSInterpreter::~KJSInterpreter()
{
    Interpreter *ip = INTERPRETER(this);
    ip->deref();
    ip = nullptr;
}

KJSContext *KJSInterpreter::globalContext()
{
    Interpreter *ip = INTERPRETER(this);

    globCtx.hnd = EXECSTATE_HANDLE(ip->globalExec());
    return &globCtx;
}

KJSObject KJSInterpreter::globalObject()
{
    Interpreter *ip = INTERPRETER(this);

    return KJSObject(JSVALUE_HANDLE(ip->globalObject()));
}

KJSResult KJSInterpreter::evaluate(const QString &sourceURL,
                                   int startingLineNumber,
                                   const QString &code,
                                   KJSObject *thisValue)
{
    Interpreter *ip = INTERPRETER(this);

    JSValue *tv = thisValue ? JSVALUE(thisValue) : nullptr;
    KJS::Completion c = ip->evaluate(toUString(sourceURL), startingLineNumber,
                                     toUString(code), tv);

    KJSResult res;
    if (c.complType() == Throw) {
        ExecState *exec = ip->globalExec();
        UString msg = c.value()->toString(exec);
#if 0
        JSObject *resObj = c.value()->toObject(exec);
        CString message = resObj->toString(exec).UTF8String();
        int line = resObj->toObject(exec)->get(exec, "line")->toUInt32(exec);

        if (!sourceURL.isEmpty()) {
            fprintf(stderr, "%s (line %d): ", qPrintable(sourceURL), line);
        }
        fprintf(stderr, "%s\n", msg.c_str());
#endif
        fprintf(stderr, "evaluate() threw an exception\n");
        res.hnd->errMsg = msg;
    } else {
        if (c.isValueCompletion()) {
            res.hnd->val = KJSObject(JSVALUE_HANDLE(c.value()));
        }
    }

    return res;
}

KJSResult KJSInterpreter::evaluate(const QString &code,
                                   KJSObject *thisValue)
{
    return evaluate(QLatin1String("<string>"), 0, code, thisValue);
}

bool KJSInterpreter::normalizeCode(const QString &code, QString *normalized,
                                   int *errLine, QString *errMsg)
{
    assert(normalized);

    UString codeOut, msg;
    bool success = Interpreter::normalizeCode(toUString(code), &codeOut,
                   errLine, &msg);

    *normalized = toQString(codeOut);
    if (errMsg) {
        *errMsg = toQString(msg);
    }

    return success;
}

