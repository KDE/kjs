// -*- c-basic-offset: 4 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2005 Apple Computer, Inc.
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

#ifndef KJS_PROPERTY_SLOT_H
#define KJS_PROPERTY_SLOT_H

#include <assert.h>
#include "identifier.h"
#include "value.h"

namespace KJS {

struct HashEntry;
class ExecState;
class JSObject;

class KJS_EXPORT PropertySlot
{
public:
    typedef JSValue *(*GetValueFunc)(ExecState *, JSObject *originalObject, const Identifier&, const PropertySlot&);
    typedef JSValue *(*GetValueNumberFunc)(ExecState *, JSObject *originalObject, unsigned, const PropertySlot&);

    JSValue *getValue(ExecState *exec, JSObject *originalObject, const Identifier& propertyName) const
    {
        switch (m_getType) {
        case ValueType:
            return m_data.jsValue;
        case ValueSlotType:
            return *m_data.valueSlot;
        case StringFunction:
        default:
            return m_func.m_getValue(exec, originalObject, propertyName, *this);
        }
    }

    JSValue *getValue(ExecState *exec, JSObject *originalObject, unsigned propertyName) const
    {
        switch (m_getType) {
        case ValueType:
            return m_data.jsValue;
        case ValueSlotType:
            return *m_data.valueSlot;
        case NumberFunction:
            return m_func.m_getValueNumber(exec, originalObject, propertyName, *this);
        case StringFunction:
        default:
            return m_func.m_getValue(exec, originalObject, Identifier::from(propertyName), *this);
        }
    }

    void setValueSlot(JSObject *slotBase, JSValue **valueSlot)
    {
        m_slotBase = slotBase;
        m_data.valueSlot = valueSlot;
        m_getType = ValueSlotType;
    }

    void setValue(JSObject *slotBase, JSValue *value)
    {
        m_slotBase = slotBase;
        m_data.jsValue = value;
        m_getType = ValueType;
    }

    void setStaticEntry(JSObject *slotBase, const HashEntry *staticEntry, GetValueFunc getValue)
    {
        assert(getValue);
        m_slotBase = slotBase;
        m_data.staticEntry = staticEntry;
        m_func.m_getValue = getValue;
        m_getType = StringFunction;
    }

    void setCustom(JSObject *slotBase, GetValueFunc getValue)
    {
        assert(getValue);
        m_slotBase = slotBase;
        m_func.m_getValue = getValue;
        m_getType = StringFunction;
    }

    void setCustomIndex(JSObject *slotBase, unsigned index, GetValueFunc getValue)
    {
        assert(getValue);
        m_slotBase = slotBase;
        m_data.index = index;
        m_func.m_getValue = getValue;
        m_getType = StringFunction;
    }

    void setCustomIndex(JSObject *slotBase, unsigned index, GetValueNumberFunc getValue)
    {
        assert(getValue);
        m_slotBase = slotBase;
        m_data.index = index;
        m_func.m_getValueNumber = getValue;
        m_getType = NumberFunction;
    }

    void setCustomValue(JSObject *slotBase, void* value, GetValueFunc getValue)
    {
        assert(getValue);
        m_slotBase = slotBase;
        m_data.value = value;
        m_func.m_getValue = getValue;
        m_getType = StringFunction;
    }

    void setGetterSlot(JSObject *slotBase, JSObject *getterFunc)
    {
        m_func.m_getValue = functionGetter;
        m_slotBase = slotBase;
        m_data.getterFunc = getterFunc;
        m_getType = StringFunction;
    }

    void setUndefined(JSObject *slotBase)
    {
        m_slotBase = slotBase;
        m_func.m_getValue = undefinedGetter;
        m_getType = StringFunction;
    }

    JSObject *slotBase() const { return m_slotBase; }

    const HashEntry *staticEntry() const { return m_data.staticEntry; }
    unsigned index() const { return m_data.index; }
    void*    customValue() const { return m_data.value; }

private:
    static JSValue *undefinedGetter(ExecState *, JSObject *, const Identifier&, const PropertySlot&);
    static JSValue *functionGetter(ExecState *, JSObject *, const Identifier&, const PropertySlot&);

    union {
        GetValueFunc m_getValue;
        GetValueNumberFunc m_getValueNumber;
    } m_func;

    JSObject *m_slotBase;
    union {
        JSObject *getterFunc;
        JSValue **valueSlot;
        const HashEntry *staticEntry;
        unsigned index;
        void*    value;
        JSValue* jsValue;
    } m_data;

    enum GetType {
        ValueType,
        ValueSlotType,
        StringFunction,
        NumberFunction
    };
    GetType m_getType;
};

}

#endif // KJS_PROPERTY_SLOT_H
