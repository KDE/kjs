/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2012 Bernd Buschinski (b.buschinski@googlemail.com)
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

#ifndef JSONSTRINGIFY_H
#define JSONSTRINGIFY_H

#include "ustring.h"
#include "identifier.h"
#include "CommonIdentifiers.h"

#include <HashSet.h>

#include <vector>

namespace KJS {

class JSValue;
class ExecState;
class JSObject;

class JSONStringify
{
public:
    enum StringifyState {
        Success,
        FailedCyclic,
        FailedException,
        FailedStackLimitExceeded
    };

    JSONStringify(ExecState* exec, JSValue* replacer, JSValue* spacer);

    JSValue* stringify(ExecState* exec, JSValue* object, StringifyState& state);

private:
    enum ReplacerType {
        Invalid,
        Function,
        Array
    };

    UString stringifyObject(KJS::ExecState* exec, KJS::JSValue* object, KJS::JSValue* propertyName, KJS::JSObject* holder);
    UString quotedString(ExecState* exec, const UString& string);
    UString stringifyValue(ExecState* exec, JSValue* object, JSValue* propertyName, JSObject* holder);

    bool isWhiteListed(const Identifier& propertyName);

    StringifyState m_state;
    ReplacerType m_replacerType;
    JSObject* m_replacerObject;
    WTF::HashSet<Identifier> m_whitelistNames;
    UString m_spacer;

    //Object Stack for cyclic detection
    std::vector<JSValue*> m_objectStack;

    bool m_rootIsUndefined;
    bool m_emtpySpacer;
};

}

#endif // JSONSTRINGIFY_H
