/*
 * Copyright (C) 2007, 2008 Apple Inc. All rights reserved.
 *           (C) 2008 Maksim Orlovich <maksim@kde.org>
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
 * Portions of this code that are (C) 2007, 2008 Apple Inc. were
 * originally distributed under the following terms
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1.  Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *   2.  Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *   3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 *   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *   DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 *   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "JSVariableObject.h"

#include "PropertyNameArray.h"
#include "property_map.h"

namespace KJS
{

bool JSVariableObject::deleteProperty(ExecState *exec, const Identifier &propertyName)
{
    if (symbolTable->contains(propertyName.ustring().rep())) {
        return false;
    }

    return JSObject::deleteProperty(exec, propertyName);
}

void JSVariableObject::getOwnPropertyNames(ExecState *exec, PropertyNameArray &propertyNames, PropertyMap::PropertyMode mode)
{
    SymbolTable::const_iterator::Keys end = symbolTable->end().keys();
    for (SymbolTable::const_iterator::Keys it = symbolTable->begin().keys(); it != end; ++it) {
        propertyNames.add(Identifier(it->get()));
    }

    JSObject::getOwnPropertyNames(exec, propertyNames, mode);
}

void JSVariableObject::mark()
{
    JSObject::mark();

    if (!localStorage) {
        return;
    }

    size_t             size    = lengthSlot();
    LocalStorageEntry *entries = localStorage;

    for (size_t i = 0; i < size; ++i) {
        JSValue *value = entries[i].val.valueVal;
        if (!(entries[i].attributes & DontMark) && !value->marked()) {
            value->mark();
        }
    }
}

} // namespace KJS
