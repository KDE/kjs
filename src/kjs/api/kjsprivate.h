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

#ifndef KJSPRIVATE_H
#define KJSPRIVATE_H

#include "kjs/ustring.h"
#include "kjs/identifier.h"
#include "kjs/list.h"
#include <QtCore/QString>

#define JSVALUE_HANDLE(v) reinterpret_cast<KJSObjectHandle*>(v)
#define JSVALUE(h) reinterpret_cast<KJS::JSValue*>((h)->hnd)

#define EXECSTATE_HANDLE(c) reinterpret_cast<KJSContextHandle*>(c)
#define EXECSTATE(ctx) reinterpret_cast<ExecState*>((ctx)->hnd)

#define INTERPRETER_HANDLE(i) reinterpret_cast<KJSInterpreterHandle*>(i)
#define INTERPRETER(h) reinterpret_cast<KJS::Interpreter*>((h)->hnd)

#define PROTOTYPE_HANDLE(p) reinterpret_cast<KJSPrototypeHandle*>(p)
#define PROTOTYPE(h) reinterpret_cast<CustomPrototype*>((h)->hnd)

#define LIST_HANDLE(l) reinterpret_cast<const KJSArgumentsHandle*>(l)
#define LIST(h) reinterpret_cast<const KJS::List*>((h)->hnd)

static inline KJS::UString toUString(const QString& s)
{
    // ### can be done faster. see khtml/ecma/kjs_binding.cpp
    int l = s.length();
    const KJS::UChar* u = reinterpret_cast<const KJS::UChar*>(s.unicode());
    return KJS::UString(u, l);
}

static inline KJS::Identifier toIdentifier(const QString& s)
{
    int l = s.length();
    const KJS::UChar* u = reinterpret_cast<const KJS::UChar*>(s.unicode());
    return KJS::Identifier(u, l);
}

static inline QString toQString(const KJS::UString& s)
{
    int l = s.size();
    const QChar* u = reinterpret_cast<const QChar*>(s.data());
    return QString(u, l);
}

#endif
