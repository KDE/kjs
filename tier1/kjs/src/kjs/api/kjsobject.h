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

#ifndef KJSOBJECT_H
#define KJSOBJECT_H

#include "kjsapi_export.h"
#include <QtCore/QString>

class QDateTime;
class KJSContext;
class KJSNull;
class KJSUndefined;
class KJSBoolean;
class KJSNumber;
class KJSString;
class KJSArray;
class KJSDate;
class KJSArguments;
class KJSInterpreter;
class KJSObjectHandle;
class KJSCustomProperty;
class KJSCustomFunction;

/**
 * A class representing a JavaScript value.
 *
 * @short Script object
 */
class KJSAPI_EXPORT KJSObject
{
    friend class KJSNull;
    friend class KJSUndefined;
    friend class KJSBoolean;
    friend class KJSNumber;
    friend class KJSString;
    friend class KJSArray;
    friend class KJSDate;
    friend class KJSGlobalObject;
    friend class KJSPrototype;
    friend class KJSContext;
    friend class KJSArguments;
    friend class KJSInterpreter;
    friend class KJSCustomProperty;
    friend class KJSCustomFunction;
public:
    /**
     * Constructs a JavaScript object of the generic Object type.
     */
    KJSObject();
    /**
     * Constructs on object from another one
     */
    KJSObject(const KJSObject& o);
    /**
     * Assigns another JS object references to this one.
     */
    KJSObject& operator=(const KJSObject& o);
    /**
     * Destructs this object freeing any references it might have held.
     */
    ~KJSObject();
    /**
     * Returns whether this object is undefined.
     */
    bool isUndefined() const;
    /**
     * Returns whether this object is null.
     */
    bool isNull() const;
    /**
     * Returns whether this object is a boolean.
     */
    bool isBoolean() const;
    /**
     * Returns whether this object is a number.
     */
    bool isNumber() const;
    /**
     * Returns whether this object is a string.
     */
    bool isString() const;
    /**
     * Returns whether this object is a full blown object.
     */
    bool isObject() const;

    /**
     * Returns this value converted to a boolean. If the conversion
     * fails the context will have an exception set.
     */
    bool toBoolean(KJSContext* ctx);
    /**
     * Returns this value converted to a number. If the conversion
     * fails the context will have an exception set.
     */
    double toNumber(KJSContext* ctx);
    /**
     * Returns this value converted to a 32-bit integer number. NaN,
     * positive and negative Infinity will be converted to 0. If the
     * conversion fails the context will have an exception set.
     */
    int toInt32(KJSContext* ctx);
    /**
     * Returns this value converted to a string. If the conversion
     * fails the context will have an exception set.
     */
    QString toString(KJSContext* ctx);
    /**
     * Reads the specified property from this object. This operation
     * might throw an exception.
     */
    KJSObject property(KJSContext* ctx, const QString& name);
    /**
     * Sets the specified property on this object. This operation
     * might throw an exception.
     */
    void setProperty(KJSContext* ctx, const QString& name,
                     const KJSObject& value);
    /**
     * @overload
     */
    void setProperty(KJSContext* ctx, const QString& name, bool value);
    /**
     * @overload
     */
    void setProperty(KJSContext* ctx, const QString& name, double value);
    /**
     * @overload
     */
    void setProperty(KJSContext* ctx, const QString& name, int value);
    /**
     * @overload
     */
    void setProperty(KJSContext* ctx, const QString& name,
                     const QString &value);
    /**
     * @overload
     *
     * Accepts a Latin1 encoded, null-terminated string.
     */
    void setProperty(KJSContext* ctx, const QString& name,
                     const char* value);


private:
    KJSObject(KJSObjectHandle* h) : hnd(h) { }
    KJSObjectHandle* hnd;
};

/**
 * A class representing a JavaScript null value.
 *
 * @short Null value
 */
class KJSAPI_EXPORT KJSNull : public KJSObject
{
public:
    /**
     * Constructs a null object.
     */
    KJSNull();
};

/**
 * A class representing an undefined JavaScript value.
 *
 * @short Undefined value
 */
class KJSAPI_EXPORT KJSUndefined : public KJSObject
{
public:
    /**
     * Constructs an undefined object.
     */
    KJSUndefined();
};

/**
 * A class representing a boolean JavaScript value.
 *
 * @short Boolean value
 */
class KJSAPI_EXPORT KJSBoolean : public KJSObject
{
public:
    /**
     * Constructs a boolean object.
     */
    KJSBoolean(bool b);
};

/**
 * A class representing a JavaScript number value.
 *
 * @short Number value
 */
class KJSAPI_EXPORT KJSNumber : public KJSObject
{
public:
    /**
     * Constructs a number object.
     */
    KJSNumber(double d);
};

/**
 * A class representing a JavaScript string value.
 *
 * @short String value
 */
class KJSAPI_EXPORT KJSString : public KJSObject
{
public:
    /**
     * Constructs a string object.
     */
    KJSString(const QString& s);
    /**
     * Constructs a string object from an Latin1 encoded,
     * null-terminated string. Note the limited input character range
     * which rules out a big part of Unicode.
     *
     * @overload
     */
    KJSString(const char* s);
};

/**
 * A class representing a JavaScript array object.
 *
 * @short Array object
 */
class KJSAPI_EXPORT KJSArray : public KJSObject
{
public:
    /**
     * Constructs an array object with the specified length.
     */
    KJSArray(KJSContext* ctx, int len = 0);
};

/**
 * A class representing a JavaScript date object.
 *
 * @short Date object
 */
class KJSAPI_EXPORT KJSDate : public KJSObject
{
public:
    /**
     * Constructs a date object from the specified date and time.
     */
    KJSDate(KJSContext* ctx, const QDateTime& dt);
};

/**
 * A class representing a global object of an execution environment.
 *
 * @short Global object
 */
class KJSAPI_EXPORT KJSGlobalObject : public KJSObject
{
    friend class KJSPrototype;
public:
    /**
     * Constructs an empty global object. Usually done through
     * KJSPrototype::constructGlobalObject().
     */
    KJSGlobalObject();

private:
    KJSGlobalObject(KJSObjectHandle* h) : KJSObject(h) { }
};

#endif
