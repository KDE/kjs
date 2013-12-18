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
 *	Redistribution and use in source and binary forms, with or without
 *	modification, are permitted provided that the following conditions
 *	 are met:
 *
 *	 1.  Redistributions of source code must retain the above copyright
 *	     notice, this list of conditions and the following disclaimer.
 *	 2.  Redistributions in binary form must reproduce the above copyright
 *	     notice, this list of conditions and the following disclaimer in the
 *	     documentation and/or other materials provided with the distribution.
 *	 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *	     its contributors may be used to endorse or promote products derived
 *	     from this software without specific prior written permission.
 *	
 *	 THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 *	 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *	 DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 *	 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *	 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *	 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *	 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *	 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *	 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef JSVariableObject_h
#define JSVariableObject_h

#include "LocalStorage.h"
#include "SymbolTable.h"
#include "object.h"

#include <wtf/Vector.h>

namespace KJS {
    class Interpreter;

    class JSVariableObject : public JSObject {
    public:
        using KJS::JSObject::deleteProperty;
        virtual bool deleteProperty(ExecState*, const Identifier&);

        virtual void getOwnPropertyNames(ExecState*, PropertyNameArray&, PropertyMap::PropertyMode mode);

        virtual void mark();

        enum {
          LengthSlot,
          TearOffNeeded, // Set when a tearoff is requested;
                         // the actual tearoff will only happen once the function
                         // stops running, though
          ScopeLink,
          NumVarObjectSlots = 3
        };

        int32_t& lengthSlot() { return localStorage[LengthSlot].val.int32Val; }
        const int32_t& lengthSlot() const { return localStorage[LengthSlot].val.int32Val; }

        bool& tearOffNeededSlot() { return localStorage[TearOffNeeded].val.boolVal; }

        ScopeChainLink& scopeLink() { return localStorage[ScopeLink].val.scopeVal; }
    protected:
        JSVariableObject(): localStorage(0), symbolTable(0) { }
        ~JSVariableObject();

        bool symbolTableGet(const Identifier&, PropertySlot&);
        bool symbolTablePut(const Identifier&, JSValue*, bool checkReadOnly);

    public:
        LocalStorageEntry* localStorage; // Storage for variables in the symbol table.
        SymbolTable*       symbolTable; // Maps name -> index in localStorage.
    };

    inline bool JSVariableObject::symbolTableGet(const Identifier& propertyName, PropertySlot& slot)
    {
        size_t index = symbolTable->get(propertyName.ustring().rep());
        if (index != missingSymbolMarker()) {
            slot.setValueSlot(this, &localStorage[index].val.valueVal);
            return true;
        }
        return false;
    }

    inline bool JSVariableObject::symbolTablePut(const Identifier& propertyName, JSValue* value, bool checkReadOnly)
    {
        size_t index = symbolTable->get(propertyName.ustring().rep());
        if (index == missingSymbolMarker())
            return false;
        LocalStorageEntry& entry = localStorage[index];
        if (checkReadOnly && (entry.attributes & ReadOnly))
            return true;
        entry.val.valueVal = value;
        return true;
    }

    inline JSVariableObject::~JSVariableObject()
    {
        if (localStorage) {
            scopeLink().deref();
            if (tearOffNeededSlot())
                delete[] localStorage;
        }
    }

    inline JSObject* ScopeChainLink::object() const
    {
        if (isToScopeChainNode())
            return asScopeChainNode()->object;
        else
            return asVariableObject();
    }

    inline ScopeChainLink ScopeChainLink::next() const
    {
        if (isToScopeChainNode())
            return asScopeChainNode()->next;
        else
            return asVariableObject()->scopeLink();
    }

    inline void ScopeChain::mark()
    {
        for (ScopeChainLink n = m_top; n.ptr; n = n.next()) {
            JSObject *o = n.object();
            if (!o->marked())
                o->mark();
        }
    }

    inline void ScopeChain::pushVariableObject(JSVariableObject* act)
    {
        // note: this assumes the new variable object is not in any
        // scope chain in the moment.

        // Set the item's next pointer to the current top.
        // there is no refcount ops since it's transferring a reference
        act->scopeLink() = m_top;

        // new top!
        m_top.set(act);
    }


    class KJS_EXPORT JSGlobalObject :  public JSObject // ### TODO: should inherit off JSVariableObject
    {
    public:
        JSGlobalObject(): m_interpreter(0) {}
        JSGlobalObject(JSValue* proto): JSObject(proto), m_interpreter(0) {}
        virtual bool isGlobalObject() const { return true; }

        void setInterpreter(Interpreter* intp) { m_interpreter = intp; }
        Interpreter* interpreter() const { return m_interpreter; }
    private:
        Interpreter* m_interpreter;
    };
} // namespace KJS

#endif // JSVariableObject_h
// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
