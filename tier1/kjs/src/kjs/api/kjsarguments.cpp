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

#include "kjsarguments.h"
#include "kjsprivate.h"

int KJSArguments::count() const
{
    const KJS::List* l = LIST(this);
    return l->size();
}

KJSObject KJSArguments::at(int idx) const
{
    const KJS::List* l = LIST(this);
    if (idx < 0 || idx >= l->size())
        return KJSUndefined();
    KJS::JSValue* a = l->at(idx);
    return KJSObject(JSVALUE_HANDLE(a));
}

