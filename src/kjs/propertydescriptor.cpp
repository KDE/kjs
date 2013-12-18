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


#include "propertydescriptor.h"
#include "object.h"
#include "operations.h"

#include <stdio.h>

namespace KJS {

PropertyDescriptor::PropertyDescriptor()
    : m_attributes(DontEnum|DontDelete|ReadOnly),
      m_setAttributes(0),
      m_value(0),
      m_getter(0),
      m_setter(0)
{
}

//ECMAScript Edition 5.1r6 - 8.10.1
bool PropertyDescriptor::isAccessorDescriptor() const
{
    return (m_getter || m_setter);
}

//ECMAScript Edition 5.1r6 - 8.10.2
bool PropertyDescriptor::isDataDescriptor() const
{
    if (!m_value && !(writableSet()))
        return false;
    return true;
}

//ECMAScript Edition 5.1r6 - 8.10.3
bool PropertyDescriptor::isGenericDescriptor() const
{
    return (!isAccessorDescriptor() && !isDataDescriptor());
}

//ECMAScript Edition 5.1r6 - 8.10.4 - FromPropertyDescriptor
JSObject* PropertyDescriptor::fromPropertyDescriptor(ExecState *exec)
{
    JSObject *ret = new JSObject(exec->lexicalInterpreter()->builtinObjectPrototype());

    if (isDataDescriptor()) {
        ret->put(exec, exec->propertyNames().writable, jsBoolean(writable()));
        ret->put(exec, exec->propertyNames().value, value() ? value() : jsUndefined());
    } else {
        ret->put(exec, exec->propertyNames().get, getter() ? getter() : jsUndefined());
        ret->put(exec, exec->propertyNames().set, setter() ? setter() : jsUndefined());
    }

    ret->put(exec, exec->propertyNames().enumerable, jsBoolean(enumerable()));
    ret->put(exec, exec->propertyNames().configurable, jsBoolean(configurable()));

    return ret;
}

//ECMAScript Edition 5.1r6 - 8.10.5 - ToPropertyDescriptor
bool PropertyDescriptor::setPropertyDescriptorFromObject(ExecState *exec, JSValue* jsValue)
{
    JSObject *obj = jsValue->getObject();
    if (!obj) {
        throwError(exec, TypeError, "not an Object");
        return false;
    }

    if (obj->hasProperty(exec, exec->propertyNames().enumerable))
        setEnumerable(obj->get(exec, exec->propertyNames().enumerable)->toBoolean(exec));

    if (obj->hasProperty(exec, exec->propertyNames().configurable))
        setConfigureable(obj->get(exec, exec->propertyNames().configurable)->toBoolean(exec));

    if (obj->hasProperty(exec, exec->propertyNames().value))
        setValue(obj->get(exec, exec->propertyNames().value));

    if (obj->hasProperty(exec, exec->propertyNames().writable))
        setWritable(obj->get(exec, exec->propertyNames().writable)->toBoolean(exec));

    if (obj->hasProperty(exec, exec->propertyNames().get)) {
        JSValue* getter = obj->get(exec, exec->propertyNames().get);
        if (!getter->isUndefined()) {
            if (!getter->implementsCall()) {
                throwError(exec, TypeError, "Getter: \'" + getter->toString(exec) + "\' is not Callable");
                return false;
            }
        }
        setGetter(getter);
    }

    if (obj->hasProperty(exec, exec->propertyNames().set)) {
        JSValue* setter = obj->get(exec, exec->propertyNames().set);
        if (!setter->isUndefined()) {
            if (!setter->implementsCall()) {
                throwError(exec, TypeError, "Setter: \'" + setter->toString(exec) + "\' is not Callable");
                return false;
            }
        }
        setSetter(setter);
    }

    if (getter() || setter()) {
        if (value() || writableSet()) {
            throwError(exec, TypeError, "can not mix accessor descriptor and data descriptor");
            return false;
        }
    }
    return true;
}

bool PropertyDescriptor::setPropertyDescriptorValues(ExecState*, JSValue* value, unsigned int attributes)
{
    setEnumerable(!(attributes & DontEnum));
    setConfigureable(!(attributes & DontDelete));

    if (!value)
        return false;
    if (value->isUndefined() || value->type() != GetterSetterType) {
        setValue(value);
        setWritable(!(attributes & ReadOnly));
    } else {
        GetterSetterImp *gs = static_cast<GetterSetterImp *>(value);
        setGetter(gs->getGetter() ? gs->getGetter() : jsUndefined());
        setSetter(gs->getSetter() ? gs->getSetter() : jsUndefined());
    }
    return true;
}

bool PropertyDescriptor::configurable() const
{
    return !(m_attributes & DontDelete);
}

bool PropertyDescriptor::enumerable() const
{
    return !(m_attributes & DontEnum);
}

bool PropertyDescriptor::writable() const
{
    return !(m_attributes & ReadOnly);
}

bool PropertyDescriptor::configureSet() const
{
    return m_setAttributes & ConfigurableSet;
}

bool PropertyDescriptor::enumerableSet() const
{
    return m_setAttributes & EnumerableSet;
}

bool PropertyDescriptor::writableSet() const
{
    return m_setAttributes & WritableSet;
}

JSValue* PropertyDescriptor::getter() const
{
    return m_getter;
}

JSValue* PropertyDescriptor::setter() const
{
    return m_setter;
}

JSValue* PropertyDescriptor::value() const
{
    return m_value;
}

void PropertyDescriptor::setEnumerable(bool enumerable)
{
    if (enumerable)
        m_attributes &= ~DontEnum;
    else
        m_attributes |= DontEnum;
    m_setAttributes |= EnumerableSet;
}

void PropertyDescriptor::setConfigureable(bool configureable)
{
    if (configureable)
        m_attributes &= ~DontDelete;
    else
        m_attributes |= DontDelete;
    m_setAttributes |= ConfigurableSet;
}

void PropertyDescriptor::setValue(JSValue* value)
{
    m_value = value;
}

void PropertyDescriptor::setWritable(bool writable)
{
    if (writable)
        m_attributes &= ~ReadOnly;
    else
        m_attributes |= ReadOnly;
    m_setAttributes |= WritableSet;
}

void PropertyDescriptor::setGetter(JSValue* getter)
{
    m_getter = getter;
    m_attributes &= ~ReadOnly;
}

void PropertyDescriptor::setSetter(JSValue* setter)
{
    m_setter = setter;
    m_attributes &= ~ReadOnly;
}

unsigned int PropertyDescriptor::attributes() const
{
    return m_attributes;
}

bool PropertyDescriptor::isEmpty() const
{
    return !m_setAttributes && !m_getter && !m_setter && !m_value;
}

inline bool compareValue(ExecState* exec, JSValue* a, JSValue* b)
{
    return (a == b || (a && b && sameValue(exec, a, b)));
}

// different from compareValue, if "own" getter/setter is missing (is 0) we are still the same
inline bool compareFunction(ExecState* exec, JSValue* a, JSValue* b)
{
    return (a == b || (b != 0 && a == 0) || (a && b && sameValue(exec, a, b)));
}

bool PropertyDescriptor::equalTo(ExecState* exec, PropertyDescriptor& other) const
{
    return (compareValue(exec, m_value, other.value()) &&
            compareFunction(exec, m_getter, other.getter()) &&
            compareFunction(exec, m_setter, other.setter()) &&
            attributes() == other.attributes());
}

unsigned int PropertyDescriptor::attributesWithOverride(PropertyDescriptor& other) const
{
    unsigned int mismatch = other.m_attributes ^ m_attributes;
    unsigned int sharedSeen = other.m_setAttributes & m_setAttributes;
    unsigned int newAttributes = m_attributes & (DontEnum|DontDelete|ReadOnly);

    if ((sharedSeen & WritableSet) && (mismatch & ReadOnly))
        newAttributes ^= ReadOnly;
    if ((sharedSeen & ConfigurableSet) && (mismatch & DontDelete))
        newAttributes ^= DontDelete;
    if ((sharedSeen & EnumerableSet) && (mismatch & DontEnum))
        newAttributes ^= DontEnum;

    return newAttributes;
}

bool PropertyDescriptor::operator==(PropertyDescriptor& other) const
{
    return (m_value == other.value() &&
            m_setter == other.setter() &&
            m_getter == other.getter() &&
            m_attributes == other.m_attributes &&
            writableSet() == other.writableSet() &&
            enumerableSet() == other.enumerableSet() &&
            configureSet() == other.configureSet());
}

}

