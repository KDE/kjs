/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003, 2004, 2005, 2007 Apple Inc. All rights reserved.
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

#ifndef KJS_VALUE_H
#define KJS_VALUE_H

#include "JSImmediate.h"
#include "ustring.h"
#include "collector.h"
#include <wtf/Noncopyable.h>
#include <stddef.h> // for size_t

#ifndef NDEBUG // protection against problems if committing with KJS_VERBOSE on

// Uncomment this to enable very verbose output from KJS
//#define KJS_VERBOSE
// Uncomment this to debug memory allocation and garbage collection
//#define KJS_DEBUG_MEM

#endif

namespace KJS
{

struct ClassInfo;
class ExecState;
class JSObject;
class JSCell;

/**
 * JSValue is the base type for all primitives (Undefined, Null, Boolean,
 * String, Number) and objects in ECMAScript.
 *
 * Note: you should never inherit from JSValue as it is for primitive types
 * only (all of which are provided internally by KJS). Instead, inherit from
 * JSObject.
 */
class KJS_EXPORT JSValue : Noncopyable
{
    friend class JSCell; // so it can derive from this class
    friend class Collector; // so it can call asCell()

private:
    JSValue();
    virtual ~JSValue();

public:
    // Querying the type.
    [[deprecated]] JSType type() const;
    static JSType type(const JSValue *value);
    [[deprecated]] bool isUndefined() const;
    static bool isUndefined(const JSValue *value);
    [[deprecated]] bool isNull() const;
    static bool isNull(const JSValue *value);
    [[deprecated]] bool isUndefinedOrNull() const;
    static bool isUndefinedOrNull(const JSValue *value);
    [[deprecated]] bool isBoolean() const;
    static bool isBoolean(const JSValue *value);
    [[deprecated]] bool isNumber() const;
    static bool isNumber(const JSValue *value);
    [[deprecated]] bool isString() const;
    static bool isString(const JSValue *value);
    [[deprecated]] bool isObject() const;
    static bool isObject(const JSValue *value);
    [[deprecated]] bool isObject(const ClassInfo *) const;
    static bool isObject(const JSValue *value, const ClassInfo *);

    // Extracting the value.
    [[deprecated]] bool getBoolean(bool &) const;
    static bool getBoolean(const JSValue *value, bool &);
    [[deprecated]] bool getBoolean() const; // false if not a boolean
    static bool getBoolean(const JSValue *value);
    [[deprecated]] bool getNumber(double &) const;
    static bool getNumber(const JSValue *value, double &);
    [[deprecated]] double getNumber() const; // NaN if not a number
    static double getNumber(const JSValue *value);
    [[deprecated]] bool getString(UString &) const;
    static bool getString(const JSValue *value, UString &);
    [[deprecated]] UString getString() const; // null string if not a string
    static UString getString(const JSValue *value); // null string if not a string
    [[deprecated]] JSObject *getObject(); // NULL if not an object
    static JSObject *getObject(JSValue *value);
    [[deprecated]] const JSObject *getObject() const; // NULL if not an object
    static const JSObject *getObject(const JSValue *value);

    // Extracting integer values.
    [[deprecated]] bool getUInt32(uint32_t &) const;
    static bool getUInt32(const JSValue *value, uint32_t &);
    [[deprecated]] bool getTruncatedInt32(int32_t &) const;
    static bool getTruncatedInt32(const JSValue *value, int32_t &v);
    [[deprecated]] bool getTruncatedUInt32(uint32_t &) const;
    static bool getTruncatedUInt32(const JSValue *value, uint32_t &v);

    [[deprecated]] JSValue *getByIndex(ExecState *exec, unsigned propertyName) const;
    static JSValue *getByIndex(const JSValue *value, ExecState *exec, unsigned propertyName);

    // Basic conversions.
    [[deprecated]] JSValue *toPrimitive(ExecState *exec, JSType preferredType = UnspecifiedType) const;
    static JSValue *toPrimitive(const JSValue *value, ExecState *exec, JSType preferredType = UnspecifiedType);
    [[deprecated]] bool getPrimitiveNumber(ExecState *exec, double &number, JSValue *&value);
    static bool getPrimitiveNumber(JSValue *that, ExecState *exec, double &number, JSValue *&value);

    [[deprecated]] bool toBoolean(ExecState *exec) const;
    static bool toBoolean(const JSValue *value, ExecState *exec);
    [[deprecated]] double toNumber(ExecState *exec) const;
    static double toNumber(const JSValue *value, ExecState *exec);
    JSValue *toJSNumber(ExecState *) const; // Fast path for when you expect that the value is an immediate number.
    [[deprecated]] UString toString(ExecState *exec) const;
    static UString toString(const JSValue *value, ExecState *exec);
    [[deprecated]] JSObject *toObject(ExecState *exec) const;
    static JSObject *toObject(const JSValue *value, ExecState *exec);

    // Integer conversions.
    [[deprecated]] double toInteger(ExecState *) const;
    static double toInteger(const JSValue *value, ExecState *);
    [[deprecated]] double toIntegerPreserveNaN(ExecState *) const;
    static double toIntegerPreserveNaN(const JSValue *value, ExecState *);
    [[deprecated]] int32_t toInt32(ExecState *) const;
    static int32_t toInt32(const JSValue *value, ExecState *exec);
    [[deprecated]] int32_t toInt32(ExecState *, bool &ok) const;
    static int32_t toInt32(const JSValue *value, ExecState *, bool &ok);
    [[deprecated]] uint32_t toUInt32(ExecState *) const;
    static uint32_t toUInt32(const JSValue *value, ExecState *);
    [[deprecated]] uint32_t toUInt32(ExecState *, bool &ok) const;
    static uint32_t toUInt32(const JSValue *value, ExecState *, bool &ok);
    [[deprecated]] uint16_t toUInt16(ExecState *exec) const;
    static uint16_t toUInt16(const JSValue *value, ExecState *exec);

    // These are identical logic to above, and faster than jsNumber(number)->toInt32(exec)
    static int32_t toInt32(double);
    static int32_t toUInt32(double);

    // Floating point conversions.
    [[deprecated]] float toFloat(ExecState *) const;
    static float toFloat(const JSValue *value, ExecState *);

    // Object-level properties...

    /**
     * Whether or not the value implements the call() method. If it does, this also
     * implies this is an object, and hence it can be cast to a JSObject
     * and the call method can be invoked
     *
     * @return true if this is an object implementing the call() method, otherwise
     * false
     */
    [[deprecated]] bool implementsCall() const;
    static bool implementsCall(const JSValue *value);

    // Garbage collection.
    [[deprecated]] void mark();
    static void mark(JSValue *value);
    [[deprecated]] bool marked() const;
    static bool marked(const JSValue *value);

    static int32_t toInt32SlowCase(double, bool &ok);
    static uint32_t toUInt32SlowCase(double, bool &ok);

private:
    static int32_t toInt32SlowCase(const JSValue *value, ExecState *, bool &ok);
    static uint32_t toUInt32SlowCase(const JSValue *value, ExecState *, bool &ok);

    // Implementation details.
    JSCell *asCell();
    const JSCell *asCell() const;

    // emulate Q_DISABLE_COPY to avoid msvc linker errors
#if !defined(_MSC_VER) || !defined(MAKE_KJS_LIB)
    // Give a compile time error if we try to copy one of these.
    JSValue(const JSValue &);
    JSValue &operator=(const JSValue &);
#endif
};

class KJS_EXPORT JSCell : public JSValue
{
    friend class Collector;
    friend class NumberImp;
    friend class StringImp;
    friend class JSObject;
    friend class GetterSetterImp;
private:
    explicit JSCell();
    ~JSCell() override;
public:
    // Querying the type.
    virtual JSType type() const = 0;
    bool isNumber() const;
    bool isString() const;
    bool isObject() const;
    bool isObject(const ClassInfo *) const;

    // Extracting the value.
    bool getNumber(double &) const;
    double getNumber() const; // NaN if not a number
    bool getString(UString &) const;
    UString getString() const; // null string if not a string
    JSObject *getObject(); // NULL if not an object
    const JSObject *getObject() const; // NULL if not an object

    // Extracting integer values.
    virtual bool getUInt32(uint32_t &) const;
    virtual bool getTruncatedInt32(int32_t &) const;
    virtual bool getTruncatedUInt32(uint32_t &) const;

    // Basic conversions.
    virtual JSValue *toPrimitive(ExecState *exec, JSType preferredType = UnspecifiedType) const = 0;
    virtual bool getPrimitiveNumber(ExecState *exec, double &number, JSValue *&value) = 0;
    virtual bool toBoolean(ExecState *exec) const = 0;
    virtual double toNumber(ExecState *exec) const = 0;
    virtual UString toString(ExecState *exec) const = 0;
    virtual JSObject *toObject(ExecState *exec) const = 0;

    // Higher-level (object-like) properties:
    virtual bool implementsCall() const;

    // Garbage collection.
    void *operator new(size_t);
    virtual void mark();
    bool marked() const;
};

KJS_EXPORT JSValue *jsNumberCell(double);

KJS_EXPORT JSCell *jsString(); // returns empty string
KJS_EXPORT JSCell *jsString(const UString &); // returns empty string if passed null string
KJS_EXPORT JSCell *jsString(const char * = ""); // returns empty string if passed 0
KJS_EXPORT JSCell *jsString(const char *s, int len);

// should be used for strings that are owned by an object that will
// likely outlive the JSValue this makes, such as the parse tree or a
// DOM object that contains a UString
JSCell *jsOwnedString(const UString &);

inline JSValue *jsUndefined()
{
    return JSImmediate::undefinedImmediate();
}

inline JSValue *jsNull()
{
    return JSImmediate::nullImmediate();
}

inline JSValue *jsNaN()
{
    static const union {
        uint64_t bits;
        double d;
    } nan = { 0x7ff80000ULL << 32 };
    return jsNumberCell(nan.d);
}

inline JSValue *jsBoolean(bool b)
{
    return b ? JSImmediate::trueImmediate() : JSImmediate::falseImmediate();
}

ALWAYS_INLINE JSValue *jsNumber(double d)
{
    JSValue *v = JSImmediate::from(d);
    return v ? v : jsNumberCell(d);
}

ALWAYS_INLINE JSValue *jsNumber(int i)
{
    JSValue *v = JSImmediate::from(i);
    return v ? v : jsNumberCell(i);
}

ALWAYS_INLINE JSValue *jsNumber(unsigned i)
{
    JSValue *v = JSImmediate::from(i);
    return v ? v : jsNumberCell(i);
}

ALWAYS_INLINE JSValue *jsNumber(long i)
{
    JSValue *v = JSImmediate::from(i);
    return v ? v : jsNumberCell(i);
}

ALWAYS_INLINE JSValue *jsNumber(unsigned long i)
{
    JSValue *v = JSImmediate::from(i);
    return v ? v : jsNumberCell(i);
}

ALWAYS_INLINE JSValue *jsNumber(long long i)
{
    JSValue *v = JSImmediate::from(i);
    return v ? v : jsNumberCell(static_cast<double>(i));
}

ALWAYS_INLINE JSValue *jsNumber(unsigned long long i)
{
    JSValue *v = JSImmediate::from(i);
    return v ? v : jsNumberCell(static_cast<double>(i));
}

ALWAYS_INLINE JSValue *jsNumberFromAnd(ExecState *exec, JSValue *v1, JSValue *v2)
{
    if (JSImmediate::areBothImmediateNumbers(v1, v2)) {
        return JSImmediate::andImmediateNumbers(v1, v2);
    }
    return jsNumber(JSValue::toInt32(v1, exec) & JSValue::toInt32(v2, exec));
}

inline JSValue::JSValue()
{
}

inline JSValue::~JSValue()
{
}

inline JSCell::JSCell()
{
}

inline JSCell::~JSCell()
{
}

inline bool JSCell::isNumber() const
{
    return type() == NumberType;
}

inline bool JSCell::isString() const
{
    return type() == StringType;
}

inline bool JSCell::isObject() const
{
    return type() == ObjectType;
}

inline bool JSCell::marked() const
{
    return Collector::isCellMarked(this);
}

inline void JSCell::mark()
{
    return Collector::markCell(this);
}

ALWAYS_INLINE JSCell *JSValue::asCell()
{
    ASSERT(!JSImmediate::isImmediate(this));
    return static_cast<JSCell *>(this);
}

ALWAYS_INLINE const JSCell *JSValue::asCell() const
{
    ASSERT(!JSImmediate::isImmediate(this));
    return static_cast<const JSCell *>(this);
}

inline bool JSValue::isUndefined() const
{
    return isUndefined(this);
}

inline bool JSValue::isUndefined(const JSValue *value)
{
    return value == jsUndefined();
}

inline bool JSValue::isNull() const
{
    return isNull(this);
}

inline bool JSValue::isNull(const JSValue *value)
{
    return value == jsNull();
}

inline bool JSValue::isUndefinedOrNull() const
{
    return isUndefinedOrNull(this);
}

inline bool JSValue::isUndefinedOrNull(const JSValue *value)
{
    return JSImmediate::isUndefinedOrNull(value);
}

inline bool JSValue::isBoolean() const
{
    return isBoolean(this);
}

inline bool JSValue::isBoolean(const JSValue *value)
{
    return JSImmediate::isBoolean(value);
}

inline bool JSValue::isNumber() const
{
    return isNumber(this);
}

inline bool JSValue::isNumber(const JSValue *value)
{
    return JSImmediate::isNumber(value) ||
           (!JSImmediate::isImmediate(value) && value->asCell()->isNumber());
}

inline bool JSValue::isString() const
{
    return isString(this);
}

inline bool JSValue::isString(const JSValue *value)
{
    return !JSImmediate::isImmediate(value) && value->asCell()->isString();
}

inline bool JSValue::isObject() const
{
    return isObject(this);
}

inline bool JSValue::isObject(const JSValue *value)
{
    return !JSImmediate::isImmediate(value) && value->asCell()->isObject();
}

inline bool JSValue::getBoolean(bool &v) const
{
    return getBoolean(this, v);
}


inline bool JSValue::getBoolean(const JSValue *value, bool &v)
{
    if (JSImmediate::isBoolean(value)) {
        v = JSImmediate::toBoolean(value);
        return true;
    }

    return false;
}

inline bool JSValue::getBoolean() const
{
    return getBoolean(this);
}

inline bool JSValue::getBoolean(const JSValue *value)
{
    return JSImmediate::isBoolean(value) ? JSImmediate::toBoolean(value) : false;
}

inline bool JSValue::getNumber(double &v) const
{
    return getNumber(this, v);
}

inline bool JSValue::getNumber(const JSValue *value, double &v)
{
    if (JSImmediate::isImmediate(value)) {
        return JSImmediate::getNumber(value, v);
    }
    return value->asCell()->getNumber(v);
}

inline double JSValue::getNumber() const
{
    return getNumber(this);
}

inline double JSValue::getNumber(const JSValue *value)
{
    return JSImmediate::isImmediate(value) ? JSImmediate::getNumber(value) : value->asCell()->getNumber();
}

inline bool JSValue::getString(UString &s) const
{
    return getString(this, s);
}

inline bool JSValue::getString(const JSValue *value, UString &s)
{
    return !JSImmediate::isImmediate(value) && value->asCell()->getString(s);
}

inline UString JSValue::getString() const
{
    return getString(this);
}

inline UString JSValue::getString(const JSValue *value)
{
    return JSImmediate::isImmediate(value) ? UString() : value->asCell()->getString();
}

inline JSObject *JSValue::getObject()
{
    return getObject(this);
}

inline const JSObject *JSValue::getObject() const
{
    return getObject(this);
}

inline JSObject *JSValue::getObject(JSValue *value)
{
    return JSImmediate::isImmediate(value) ? nullptr : value->asCell()->getObject();
}

inline const JSObject *JSValue::getObject(const JSValue *value)
{
    return JSImmediate::isImmediate(value) ? nullptr : value->asCell()->getObject();
}

ALWAYS_INLINE bool JSValue::getUInt32(uint32_t &v) const
{
    return getUInt32(this, v);
}

ALWAYS_INLINE bool JSValue::getUInt32(const JSValue *value, uint32_t &v)
{
    return JSImmediate::isImmediate(value) ? JSImmediate::getUInt32(value, v) : value->asCell()->getUInt32(v);
}

ALWAYS_INLINE bool JSValue::getTruncatedInt32(int32_t &v) const
{
    return getTruncatedInt32(this, v);
}

ALWAYS_INLINE bool JSValue::getTruncatedInt32(const JSValue *value, int32_t &v)
{
    return JSImmediate::isImmediate(value) ? JSImmediate::getTruncatedInt32(value, v) : value->asCell()->getTruncatedInt32(v);
}

inline bool JSValue::getTruncatedUInt32(uint32_t &v) const
{
    return getTruncatedUInt32(this, v);
}

inline bool JSValue::getTruncatedUInt32(const JSValue *value, uint32_t &v)
{
    return JSImmediate::isImmediate(value) ? JSImmediate::getTruncatedUInt32(value, v) : value->asCell()->getTruncatedUInt32(v);
}

inline void JSValue::mark()
{
    mark(this);
}

inline void JSValue::mark(JSValue *value)
{
    ASSERT(!JSImmediate::isImmediate(value)); // callers should check !marked() before calling mark()
    value->asCell()->mark();
}

inline bool JSValue::marked() const
{
    return marked(this);
}

inline bool JSValue::marked(const JSValue *value)
{
    return JSImmediate::isImmediate(value) || value->asCell()->marked();
}

inline JSType JSValue::type() const
{
    return type(this);
}

inline JSType JSValue::type(const JSValue *value)
{
    return JSImmediate::isImmediate(value) ? JSImmediate::type(value) : value->asCell()->type();
}

inline JSValue *JSValue::toPrimitive(ExecState *exec, JSType preferredType) const
{
    return toPrimitive(this, exec, preferredType);
}

inline JSValue *JSValue::toPrimitive(const JSValue *value, ExecState *exec, JSType preferredType)
{
    return JSImmediate::isImmediate(value) ? const_cast<JSValue *>(value) : value->asCell()->toPrimitive(exec, preferredType);
}

inline bool JSValue::getPrimitiveNumber(ExecState *exec, double &number, JSValue *&value)
{
    return getPrimitiveNumber(this, exec, number, value);
}

inline bool JSValue::getPrimitiveNumber(JSValue *that, ExecState *exec, double &number, JSValue *&value)
{
    if (JSImmediate::isImmediate(that)) {
        number = JSImmediate::toDouble(that);
        value = that;
        return true;
    }
    return that->asCell()->getPrimitiveNumber(exec, number, value);

}

inline bool JSValue::toBoolean(ExecState *exec) const
{
    return toBoolean(this, exec);
}

inline bool JSValue::toBoolean(const JSValue *value, ExecState *exec)
{
    return JSImmediate::isImmediate(value) ? JSImmediate::toBoolean(value) : value->asCell()->toBoolean(exec);
}

ALWAYS_INLINE double JSValue::toNumber(ExecState *exec) const
{
    return toNumber(this, exec);
}

ALWAYS_INLINE double JSValue::toNumber(const JSValue *value, ExecState *exec)
{
    return JSImmediate::isImmediate(value) ? JSImmediate::toDouble(value) : value->asCell()->toNumber(exec);
}

ALWAYS_INLINE JSValue *JSValue::toJSNumber(ExecState *exec) const
{
    return JSImmediate::isNumber(this) ? const_cast<JSValue *>(this) : jsNumber(toNumber(this, exec));
}

inline UString JSValue::toString(ExecState *exec) const
{
    return toString(this, exec);
}

inline UString JSValue::toString(const JSValue *value, ExecState *exec)
{
    return JSImmediate::isImmediate(value) ? JSImmediate::toString(value) : value->asCell()->toString(exec);
}

inline JSObject *JSValue::toObject(ExecState *exec) const
{
    return toObject(this, exec);
}

inline JSObject *JSValue::toObject(const JSValue *value, ExecState *exec)
{
    return JSImmediate::isImmediate(value) ? JSImmediate::toObject(value, exec) : value->asCell()->toObject(exec);
}

ALWAYS_INLINE int32_t JSValue::toInt32(ExecState *exec) const
{
    return toInt32(this, exec);
}

ALWAYS_INLINE int32_t JSValue::toInt32(const JSValue *value, ExecState *exec)
{
    int32_t i;
    if (getTruncatedInt32(value, i)) {
        return i;
    }
    bool ok;
    return toInt32SlowCase(value, exec, ok);
}

inline uint32_t JSValue::toUInt32(ExecState *exec) const
{
    return toUInt32(this, exec);
}

inline uint32_t JSValue::toUInt32(const JSValue *value, ExecState *exec)
{
    uint32_t i;
    if (getTruncatedUInt32(value, i)) {
        return i;
    }
    bool ok;
    return toUInt32SlowCase(value, exec, ok);
}

inline int32_t JSValue::toInt32(double val)
{
    if (!(val >= -2147483648.0 && val < 2147483648.0)) {
        bool ignored;
        return toInt32SlowCase(val, ignored);
    }
    return static_cast<int32_t>(val);
}

inline int32_t JSValue::toUInt32(double val)
{
    if (!(val >= 0.0 && val < 4294967296.0)) {
        bool ignored;
        return toUInt32SlowCase(val, ignored);
    }
    return static_cast<uint32_t>(val);
}

inline int32_t JSValue::toInt32(ExecState *exec, bool &ok) const
{
    return toInt32(this, exec, ok);
}

inline int32_t JSValue::toInt32(const JSValue *value, ExecState *exec, bool &ok)
{
    int32_t i;
    if (getTruncatedInt32(value, i)) {
        ok = true;
        return i;
    }
    return toInt32SlowCase(value, exec, ok);
}

inline uint32_t JSValue::toUInt32(ExecState *exec, bool &ok) const
{
    return toUInt32(this, exec, ok);
}

inline uint32_t JSValue::toUInt32(const JSValue *value, ExecState *exec, bool &ok)
{
    uint32_t i;
    if (getTruncatedUInt32(value, i)) {
        ok = true;
        return i;
    }
    return toUInt32SlowCase(value, exec, ok);
}

inline bool JSValue::implementsCall() const
{
    return implementsCall(this);
}

inline bool JSValue::implementsCall(const JSValue *value)
{
    if (JSImmediate::isImmediate(value)) {
        return false;    // immediate values are never calleable.
    } else {
        return value->asCell()->implementsCall();
    }

}

} // namespace

#endif // KJS_VALUE_H
