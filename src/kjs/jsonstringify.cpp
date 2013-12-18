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


#include "jsonstringify.h"

#include <algorithm>

#include "object.h"
#include "operations.h"
#include "array_instance.h"
#include "number_object.h"
#include "bool_object.h"
#include "string_object.h"
#include "function.h"

#include "wtf/Assertions.h"


namespace KJS {

static const unsigned int StackObjectLimit = 1500;

JSONStringify::JSONStringify(ExecState* exec, JSValue* replacer, JSValue* spacer)
    : m_state(Success)
{
    m_replacerObject = replacer->getObject();

    if (!m_replacerObject)
        m_replacerType = Invalid;
    else if (replacer->implementsCall())
        m_replacerType = Function;
    else if (m_replacerObject->inherits(&ArrayInstance::info)) {
        //get all whitelist names
        m_replacerType = Array;
        PropertyNameArray names;
        m_replacerObject->getOwnPropertyNames(exec, names, PropertyMap::ExcludeDontEnumProperties);
        const int size = names.size();
        bool isValidIndex = false;
        for (int i = 0; i < size; ++i) {
            names[i].toArrayIndex(&isValidIndex);
            if (!isValidIndex)
                continue;
            m_whitelistNames.add(Identifier(m_replacerObject->get(exec, names[i])->toString(exec)));
            if (exec->hadException()) {
                m_state = FailedException;
                return;
            }
        }
    } else {
        m_replacerType = Invalid;
        m_replacerObject = 0;
    }

    JSObject* spacerObject = spacer->getObject();
    m_emtpySpacer = true;
    if (spacer->isString() || (spacerObject && spacerObject->inherits(&StringInstance::info))) {
        m_spacer = spacer->toString(exec);
        if (exec->hadException()) {
            m_state = FailedException;
            return;
        }
        if (!m_spacer.isEmpty()) {
            m_spacer = m_spacer.substr(0, 10);
            m_emtpySpacer = false;
        }
    } else if (spacer->isNumber() || (spacerObject && spacerObject->inherits(&NumberInstance::info))) {
        double spacesDouble = spacer->toInteger(exec);
        if (exec->hadException()) {
            m_state = FailedException;
            return;
        }

        int spaces;
        if (isNaN(spacesDouble) || isInf(spacesDouble))
            spaces = 0;
        else
            spaces = static_cast<int>(spacesDouble);

        if (spaces > 0) {
            m_emtpySpacer = false;
            int max = std::min<int>(spaces, 10);
            for (int i = 0; i < max; ++i)
                m_spacer.append(' ');
        }
    }
    m_rootIsUndefined = false;
}

JSValue* JSONStringify::stringify(ExecState* exec, JSValue* object, StringifyState& state)
{
    JSObject* holder = static_cast<JSObject *>(exec->lexicalInterpreter()->builtinObject()->construct(exec, List::empty()));
    UString ret = stringifyValue(exec, object, jsString(""), holder);
    state = m_state;

    if (m_rootIsUndefined)
        return jsUndefined();

    if (m_state == Success)
        return jsString(ret);
    return jsUndefined();
}

UString JSONStringify::quotedString(ExecState* exec, const UString& string)
{
    //Check if we already failed
    if (m_state != Success)
        return UString();

    if (exec->hadException()) {
        m_state = FailedException;
        return UString();
    }

    const int size = string.size();
    UString ret = "\"";

    for (int i = 0; i < size; ++i) {
        int start = i;
        static const short unsigned blackSlashUC = '\\';
        static const short unsigned quoteUC = '\"';
        while (i < size && (string[i].uc > 0x001F && string[i].uc != blackSlashUC && string[i].uc != quoteUC))
            ++i;
        ret += string.substr(start, i-start);

        if (i >= size)
            break;

        switch (string[i].uc) {
            case '\t':
                ret += "\\t";
                break;
            case '\r':
                ret += "\\r";
                break;
            case '\n':
                ret += "\\n";
                break;
            case '\f':
                ret += "\\f";
                break;
            case '\b':
                ret += "\\b";
                break;
            case '"':
                ret += "\\\"";
                break;
            case '\\':
                ret += "\\\\";
                break;
            default:
                static const char hexDigits[] = "0123456789abcdef";
                short unsigned ch = string[i].uc;
                ret.append("\\u");
                ret.append(hexDigits[(ch >> 12) & 0xF]);
                ret.append(hexDigits[(ch >> 8) & 0xF]);
                ret.append(hexDigits[(ch >> 4) & 0xF]);
                ret.append(hexDigits[ch & 0xF]);
                break;
        }
    }

    ret.append('\"');
    return ret;
}

bool JSONStringify::isWhiteListed(const Identifier& propertyName)
{
    if (m_replacerType != Array)
        return true;

    return m_whitelistNames.contains(propertyName);
}

UString JSONStringify::stringifyObject(KJS::ExecState* exec, KJS::JSValue* object, KJS::JSValue* propertyName, KJS::JSObject* /*holder*/)
{
    if (m_state != Success)
        return UString();

    // As stringifyObject is only called with object->type() == ObhectType, this can't be null
    JSObject* jso = object->getObject();

    if (jso->hasProperty(exec, exec->propertyNames().toJSON)) {
        JSObject* toJSONFunc = 0;
        toJSONFunc = jso->get(exec, exec->propertyNames().toJSON)->getObject();

        if (toJSONFunc) {
            m_objectStack.push_back(object);
            List args;
            args.append(propertyName);
            JSValue* toJSONCall = toJSONFunc->call(exec, jso, args);
            if (exec->hadException()) {
                m_state = FailedException;
                return UString();
            }

            //Check if the toJSON call returned a function
            // we check it here because our stack already contains an object,
            // but this is still the root object.
            if (m_objectStack.size() == 1 && toJSONCall->implementsCall()) {
                m_rootIsUndefined = true;
                return UString();
            }

            return stringifyValue(exec, toJSONCall, propertyName, jso);
        }
    }

    if (jso->inherits(&BooleanInstance::info)) {
        return jso->toString(exec);
    } else if (jso->inherits(&NumberInstance::info)) {
        double val = jso->toNumber(exec);
        if (isInf(val) || isNaN(val)) // !isfinite
            return UString("null");
        return UString::from(val);
    } else if (jso->inherits(&StringInstance::info)) {
        return quotedString(exec, jso->toString(exec));
    } else if (jso->implementsCall()) {
        return UString("null");
    } else if (jso->inherits(&ArrayInstance::info)) { //stringify array object
        m_objectStack.push_back(object);
        PropertyNameArray names;
        jso->getPropertyNames(exec, names, KJS::PropertyMap::ExcludeDontEnumProperties);
        const int size = names.size();
        if (size == 0)
            return UString("[]");

        //filter names
        PropertyNameArray whiteListedNames;
        bool isValidIndex = false;
        for (int i = 0; i < size; ++i) {
            if (isWhiteListed(names[i])) {
                names[i].toArrayIndex(&isValidIndex);
                if (isValidIndex)
                    whiteListedNames.add(names[i]);
            }
        }
        const int sizeWhitelisted = whiteListedNames.size();
        if (sizeWhitelisted == 0)
            return UString("[]");

        UString ret = "[";
        for (int i = 0; i < sizeWhitelisted; ++i) {
            JSValue* arrayVal = jso->get(exec, whiteListedNames[i]);
            //do not render undefined, ECMA Edition 5.1r6 - 15.12.3 NOTE 2
            if (arrayVal->isUndefined())
                continue;

            if (!m_emtpySpacer) {
                ret.append('\n');
                ret += m_spacer;
            }
            ret += stringifyValue(exec, arrayVal, propertyName, jso);
            if (m_state != Success)
                return UString();
            if (i != sizeWhitelisted-1)
                ret.append(',');
        }

        if (!m_emtpySpacer)
            ret.append('\n');

        ret.append(']');
        m_objectStack.pop_back();
        return ret;
    } else { //stringify real object
        m_objectStack.push_back(object);
        PropertyNameArray names;
        jso->getPropertyNames(exec, names, KJS::PropertyMap::ExcludeDontEnumProperties);
        const int size = names.size();
        if (size == 0)
            return UString("{}");

        //filter names
        PropertyNameArray whiteListedNames;
        for (int i = 0; i < size; ++i) {
            if (isWhiteListed(names[i]))
                whiteListedNames.add(names[i]);
        }
        const int sizeWhitelisted = whiteListedNames.size();
        if (sizeWhitelisted == 0)
            return UString("{}");

        UString ret = "{";
        for (int i = 0; i < sizeWhitelisted; ++i) {
            JSValue* objectVal = jso->get(exec, whiteListedNames[i]);
            //do not render undefined, ECMA Edition 5.1r6 - 15.12.3 NOTE 2
            if (objectVal->isUndefined())
                continue;

            if (!m_emtpySpacer) {
                ret.append('\n');
                ret += m_spacer;
            }
            ret += quotedString(exec, whiteListedNames[i].ustring());
            ret += ":";
            ret += stringifyValue(exec, objectVal, jsString(whiteListedNames[i].ustring()), jso);
            if (m_state != Success)
                return UString();
            if (i != sizeWhitelisted-1)
                ret.append(',');
        }

        if (!m_emtpySpacer)
            ret.append('\n');

        ret.append('}');
        m_objectStack.pop_back();
        return ret;
    }
    return UString("null");
}

UString JSONStringify::stringifyValue(KJS::ExecState* exec, KJS::JSValue* object, KJS::JSValue* propertyName, KJS::JSObject* holder)
{
    //Check if we already failed
    if (m_state != Success)
        return UString();

    if (exec->hadException()) {
        m_state = FailedException;
        return UString();
    }

    if (m_objectStack.size() > StackObjectLimit) {
        m_state = FailedStackLimitExceeded;
        return UString();
    }

    if (!m_objectStack.empty()) {
        std::vector<JSValue*>::iterator found = std::find(m_objectStack.begin(), m_objectStack.end(), object);
        if (found != m_objectStack.end()) {
            m_state = FailedCyclic;
            return UString();
        }
    }

    if (m_replacerObject && m_replacerType == Function) {
        List args;
        args.append(propertyName);
        args.append(object);
        object = m_replacerObject->call(exec, holder, args);
        if (exec->hadException()) {
            m_state = FailedException;
            return UString();
        }
    }

    //Check if root object is a function, after replace
    if (m_objectStack.empty() && object->implementsCall()) {
        m_rootIsUndefined = true;
        return UString();
    }

    JSType type = object->type();
    switch (type) {
        case ObjectType:
            return stringifyObject(exec, object, propertyName, holder);
        case NumberType: {
            double val = object->getNumber();
            if (isInf(val) || isNaN(val)) // !isfinite
                return UString("null");
            // fall through
        }
        case BooleanType:
            return object->toString(exec);
        case StringType:
            return quotedString(exec, object->toString(exec));
            break;
        case UndefinedType:
            // Special case: while we normally don't render undefined,
            // this is not the case if our "root" object is undefined,
            // or replaced to undefined.
            // Hence check if root object, AFTER REPLACE, is undefined.
            if (m_objectStack.empty()) {
                m_rootIsUndefined = true;
                return UString();
            }
            // beside from root Object we should never render Undefined
            ASSERT_NOT_REACHED();
        case NullType:
        case UnspecifiedType:
        case GetterSetterType:
        default:
            return UString("null");
    }
    ASSERT_NOT_REACHED();
    return UString("null");
}

};
