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

#include "kjsobject.h"
#include "kjsprivate.h"
#include "kjscontext.h"
#include "kjs/value.h"
#include "kjs/object.h"
#include "kjs/protect.h"
#include "kjs/ExecState.h"
#include "kjs/JSVariableObject.h"

#include <QDebug>
#include <QDateTime>

using namespace KJS;

KJSObject::KJSObject()
    : hnd(JSVALUE_HANDLE(new JSObject()))
{
    gcProtect(JSVALUE(this));
}

KJSObject::KJSObject(const KJSObject &o)
    : hnd(o.hnd)
{
    gcProtectNullTolerant(JSVALUE(this));
}

KJSObject &KJSObject::operator=(const KJSObject &o)
{
    gcUnprotectNullTolerant(JSVALUE(this));

    hnd = o.hnd;
    gcProtectNullTolerant(JSVALUE(this));

    return *this;
}

KJSObject::~KJSObject()
{
    gcUnprotectNullTolerant(JSVALUE(this));
}

KJSNull::KJSNull()
    : KJSObject(JSVALUE_HANDLE(jsNull()))
{
}

KJSUndefined::KJSUndefined()
    : KJSObject(JSVALUE_HANDLE(jsUndefined()))
{
}

KJSBoolean::KJSBoolean(bool b)
    : KJSObject(JSVALUE_HANDLE(jsBoolean(b)))
{
}

KJSNumber::KJSNumber(double d)
    : KJSObject(JSVALUE_HANDLE(jsNumber(d)))
{
    gcProtect(JSVALUE(this));
}

KJSString::KJSString(const QString &s)
    : KJSObject(JSVALUE_HANDLE(jsString(toUString(s))))
{
    gcProtect(JSVALUE(this));
}

KJSString::KJSString(const char *s)
    : KJSObject(JSVALUE_HANDLE(jsString(s)))
{
    gcProtect(JSVALUE(this));
}

static JSValue *constructArrayHelper(ExecState *exec, int len)
{
    JSObject *builtinArray = exec->lexicalInterpreter()->builtinArray();
    JSObject *newArray = builtinArray->construct(exec, List());
    newArray->put(exec, exec->propertyNames().length, jsNumber(len),
                  DontDelete | ReadOnly | DontEnum);
    return newArray;
}

KJSArray::KJSArray(KJSContext *ctx, int len)
    : KJSObject(JSVALUE_HANDLE(constructArrayHelper(EXECSTATE(ctx), len)))

{
    gcProtect(JSVALUE(this));
}

static JSValue *constructDateHelper(KJSContext *ctx, const QDateTime &dt)
{
    Q_UNUSED(ctx);
    Q_UNUSED(dt);
    qWarning() << "converDateTimeHelper() not implemented, yet";

    // ### make call into data_object.cpp

    return jsNumber(42.0);
}

KJSDate::KJSDate(KJSContext *ctx, const QDateTime &dt)
    : KJSObject(JSVALUE_HANDLE(constructDateHelper(ctx, dt)))
{
    gcProtect(JSVALUE(this));
}

bool KJSObject::isUndefined() const
{
    return JSVALUE(this)->isUndefined();
}

bool KJSObject::isNull() const
{
    return JSVALUE(this)->isNull();
}

bool KJSObject::isBoolean() const
{
    return JSVALUE(this)->isBoolean();
}

bool KJSObject::isNumber() const
{
    return JSVALUE(this)->isNumber();
}

bool KJSObject::isString() const
{
    return JSVALUE(this)->isString();
}

bool KJSObject::isObject() const
{
    return JSVALUE(this)->isObject();
}

bool KJSObject::toBoolean(KJSContext *ctx)
{
    ExecState *exec = EXECSTATE(ctx);
    assert(exec);
    return JSVALUE(this)->toBoolean(exec);
}

double KJSObject::toNumber(KJSContext *ctx)
{
    ExecState *exec = EXECSTATE(ctx);
    assert(exec);
    return JSVALUE(this)->toNumber(exec);
}

int KJSObject::toInt32(KJSContext *ctx)
{
    ExecState *exec = EXECSTATE(ctx);
    assert(exec);
    return JSVALUE(this)->toInt32(exec);
}

QString KJSObject::toString(KJSContext *ctx)
{
    ExecState *exec = EXECSTATE(ctx);
    assert(exec);
    return toQString(JSVALUE(this)->toString(exec));
}

KJSObject KJSObject::property(KJSContext *ctx, const QString &name)
{
    JSValue *v = JSVALUE(this);
    assert(v);

#if 0
    // would normally throw an exception
    if (v == jsUndefined || v == jsNull()) {
        return KJSUndefined();
    }
#endif

    ExecState *exec = EXECSTATE(ctx);
    JSObject *o = v->toObject(exec);
    JSValue *res = o->get(exec, toIdentifier(name));

    return KJSObject(JSVALUE_HANDLE(res));
}

void KJSObject::setProperty(KJSContext *ctx, const QString &name,
                            const KJSObject &value)
{
    JSValue *v = JSVALUE(this);
    assert(v);

    ExecState *exec = EXECSTATE(ctx);
    JSObject *o = v->toObject(exec);
    o->put(exec, toIdentifier(name), JSVALUE(&value));
}

void KJSObject::setProperty(KJSContext *ctx, const QString &name, bool value)
{
    setProperty(ctx, name, KJSBoolean(value));
}

void KJSObject::setProperty(KJSContext *ctx, const QString &name,
                            double value)
{
    setProperty(ctx, name, KJSNumber(value));
}

void KJSObject::setProperty(KJSContext *ctx, const QString &name,
                            int value)
{
    setProperty(ctx, name, KJSNumber(double(value)));
}

void KJSObject::setProperty(KJSContext *ctx, const QString &name,
                            const QString &value)
{
    setProperty(ctx, name, KJSString(value));
}

void KJSObject::setProperty(KJSContext *ctx, const QString &name,
                            const char *value)
{
    setProperty(ctx, name, KJSString(value));
}

KJSGlobalObject::KJSGlobalObject(): KJSObject(JSVALUE_HANDLE(new JSGlobalObject()))
{
}
