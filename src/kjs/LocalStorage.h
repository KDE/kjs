/*
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2003, 2006, 2007, 2008 Apple Inc. All rights reserved.
 *  Copyright (C) 2007 Cameron Zwarich (cwzwarich@uwaterloo.ca)
 *  Copyright (C) 2007 Maks Orlovich
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

#ifndef KJS_LOCAL_STORAGE_H
#define KJS_LOCAL_STORAGE_H

#include "kjs/global.h"

#if HAVE_STDINT_H
#include <stdint.h>   // int32_t
#endif
#include <wtf/Vector.h>
#include <wtf/VectorTraits.h>
#include "scope_chain.h" // for ScopeChainLink

namespace KJS
{
class JSValue;

struct LocalStorageEntry {
    LocalStorageEntry()
    {
    }

    LocalStorageEntry(JSValue *v, unsigned a) :
        attributes(a)
    {
        val.valueVal = v;
    }

    union {
        double   numberVal; //### TODO: use 2 entries for this on 32-bit..
        JSValue *valueVal;
        bool     boolVal;
        int32_t  int32Val;
        ScopeChainLink scopeVal;
    } val;
    unsigned attributes;
};

typedef Vector<LocalStorageEntry, 32> LocalStorage;
}

namespace WTF
{
template<> struct VectorTraits<KJS::LocalStorageEntry> : VectorTraitsBase<true, KJS::LocalStorageEntry> { };
}

#endif // KJS_LOCAL_STORAGE_H
