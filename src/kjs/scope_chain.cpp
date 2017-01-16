/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2003 Apple Computer, Inc.
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

#include "scope_chain.h"
#include "PropertyNameArray.h"
#include "object.h"
#include "JSVariableObject.h"

#include <stdio.h>

namespace KJS
{

#ifndef NDEBUG

void ScopeChain::print()
{
    ScopeChainIterator scopeEnd = end();
    for (ScopeChainIterator scopeIter = begin(); scopeIter != scopeEnd; ++scopeIter) {
        JSObject *o = *scopeIter;
        PropertyNameArray propertyNames;
        // FIXME: should pass ExecState here!
        o->getPropertyNames(nullptr, propertyNames);
        PropertyNameArrayIterator propEnd = propertyNames.end();

        fprintf(stderr, "----- [scope %p] -----\n", (void *)o);
        for (PropertyNameArrayIterator propIter = propertyNames.begin(); propIter != propEnd; ++propIter) {
            Identifier name = *propIter;
            fprintf(stderr, "%s, ", name.ascii());
        }
        fprintf(stderr, "\n");
    }
}

#endif

} // namespace KJS
