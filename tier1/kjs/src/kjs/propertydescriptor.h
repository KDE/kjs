/*
    This file is part of the KDE libraries
    Copyright (C) 2012  Bernd Buschinski <b.buschinski@googlemail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef PROPERTYDESCRIPTOR_H
#define PROPERTYDESCRIPTOR_H

#include "global.h"
#include "ustring.h"

namespace KJS {

class JSObject;
class ExecState;

class KJS_EXPORT PropertyDescriptor
{
public:
    PropertyDescriptor();

    bool isAccessorDescriptor() const;
    bool isDataDescriptor() const;
    bool isGenericDescriptor() const;
    JSObject* fromPropertyDescriptor(ExecState* exec);
    // Set the PropertyDescriptor given Javascript Object containing any of
    // value, get, set, enumerable, configurable, writeable
    bool setPropertyDescriptorFromObject(ExecState* exec, JSValue* obj);
    // Set the PropertyDescriptor from internal Object, given the value, which can be
    // a GetterSetterImpl and set attributes
    bool setPropertyDescriptorValues(ExecState* exec, JSValue *value, unsigned int attributes);

    bool enumerable() const;
    bool writable() const;
    bool configurable() const;

    // enumerableSet & co, true if setPropertyDescriptorFromObject contained
    // enumerable, configurable or writeable, if not false.
    bool enumerableSet() const;
    bool writableSet() const;
    bool configureSet() const;

    JSValue* value() const;
    JSValue* getter() const;
    JSValue* setter() const;

    void setEnumerable(bool enumerable);
    void setConfigureable(bool configureable);
    void setValue(JSValue* value);
    void setWritable(bool writable);
    void setGetter(JSValue* getter);
    void setSetter(JSValue* setter);

    unsigned int attributes() const;

    bool isEmpty() const;

    // Comapred PropertyDescriptor in terms of its value. It compared the Attributes
    // but not if these values are explicitly set. Also the check is difference
    // in comparing the getter/setter. They are compared if they need to be updated,
    // not only if they have the same value.
    bool equalTo(ExecState* exec, PropertyDescriptor& other) const;

    // This function gives new Attributes calculation from current and other
    // PropertyDescriptor. New Attributes are set depending if Descriptor has
    // enumerable/writeable/configurableSet, if absent default is used.
    // NOTE: As interval have enumerable/writable/configurable always set and
    // javascript object dont, the order matters here.
    // In this case the correct order is: current.attributesWithOverride(new)
    // where new is the javascript object that might not have all attributes set.
    unsigned int attributesWithOverride(PropertyDescriptor& other) const;

private:
    // Check if PropertyDescriptor really is the same. This is private for
    // internal use only, so that it will not easily be confussed with equalTo.
    // This function does compared set Attributes.
    bool operator==(PropertyDescriptor& other) const;

    unsigned int m_attributes;
    unsigned int m_setAttributes;
    enum { WritableSet = 1 << 0, EnumerableSet = 1 << 1, ConfigurableSet = 1 << 2 };

    JSValue* m_value;
    JSValue* m_getter;
    JSValue* m_setter;
};

}

#endif // PROPERTYDESCRIPTOR_H
