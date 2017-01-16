// krazy:excludeall=doublequote_chars (UStrings aren't QStrings)
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000,2003 Harri Porten (porten@kde.org)
 *  Copyright (C) 2007 Apple Inc. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 *
 */

#include "number_object.h"
#include "number_object.lut.h"

#include "dtoa.h"
#include "error_object.h"
#include "operations.h"
#include <wtf/Assertions.h>
#include <wtf/MathExtras.h>
#include <wtf/Vector.h>

namespace KJS
{

// GCC cstring uses these automatically, but not all implementations do.
using std::strlen;
using std::strcpy;
using std::strncpy;
using std::memset;
using std::memcpy;

static const double MAX_SAFE_INTEGER = 9007199254740991.0; //(2^53)-1
static const double MIN_SAFE_INTEGER = -9007199254740991.0; //-((2^53)-1)

// ------------------------------ NumberInstance ----------------------------

const ClassInfo NumberInstance::info = {"Number", nullptr, nullptr, nullptr};

NumberInstance::NumberInstance(JSObject *proto)
    : JSWrapperObject(proto)
{
}

JSObject *NumberInstance::valueClone(Interpreter *targetCtx) const
{
    NumberInstance *copy = new NumberInstance(targetCtx->builtinNumberPrototype());
    copy->setInternalValue(internalValue());
    return copy;
}

// ------------------------------ NumberPrototype ---------------------------

// ECMA 15.7.4

NumberPrototype::NumberPrototype(ExecState *exec, ObjectPrototype *objProto, FunctionPrototype *funcProto)
    : NumberInstance(objProto)
{
    setInternalValue(jsNumber(0));

    // The constructor will be added later, after NumberObjectImp has been constructed

    putDirectFunction(new NumberProtoFunc(exec, funcProto, NumberProtoFunc::ToString,       1, exec->propertyNames().toString), DontEnum);
    putDirectFunction(new NumberProtoFunc(exec, funcProto, NumberProtoFunc::ToLocaleString, 0, exec->propertyNames().toLocaleString), DontEnum);
    putDirectFunction(new NumberProtoFunc(exec, funcProto, NumberProtoFunc::ValueOf,        0, exec->propertyNames().valueOf), DontEnum);
    putDirectFunction(new NumberProtoFunc(exec, funcProto, NumberProtoFunc::ToFixed,        1, exec->propertyNames().toFixed), DontEnum);
    putDirectFunction(new NumberProtoFunc(exec, funcProto, NumberProtoFunc::ToExponential,  1, exec->propertyNames().toExponential), DontEnum);
    putDirectFunction(new NumberProtoFunc(exec, funcProto, NumberProtoFunc::ToPrecision,    1, exec->propertyNames().toPrecision), DontEnum);
}

// ------------------------------ NumberProtoFunc ---------------------------

NumberProtoFunc::NumberProtoFunc(ExecState *exec, FunctionPrototype *funcProto, int i, int len, const Identifier &name)
    : InternalFunctionImp(funcProto, name)
    , id(i)
{
    putDirect(exec->propertyNames().length, len, DontDelete | ReadOnly | DontEnum);
}

static UString integer_part_noexp(double d)
{
    int decimalPoint;
    int sign;
    char *result = kjs_dtoa(d, 0, 0, &decimalPoint, &sign, nullptr);
    bool resultIsInfOrNan = (decimalPoint == 9999);
    size_t length = strlen(result);

    UString str = sign ? "-" : "";
    if (resultIsInfOrNan) {
        str += result;
    } else if (decimalPoint <= 0) {
        str += "0";
    } else {
        Vector<char, 1024> buf(decimalPoint + 1);

        if (static_cast<int>(length) <= decimalPoint) {
            strcpy(buf.data(), result);
            memset(buf.data() + length, '0', decimalPoint - length);
        } else {
            strncpy(buf.data(), result, decimalPoint);
        }

        buf[decimalPoint] = '\0';
        str += UString(buf.data());
    }

    kjs_freedtoa(result);

    return str;
}

static UString char_sequence(char c, int count)
{
    Vector<char, 2048> buf(count + 1, c);
    buf[count] = '\0';

    return UString(buf.data());
}

static double intPow10(int e)
{
    // This function uses the "exponentiation by squaring" algorithm and
    // long double to quickly and precisely calculate integer powers of 10.0.

    // This is a handy workaround for <rdar://problem/4494756>

    if (e == 0) {
        return 1.0;
    }

    bool negative = e < 0;
    unsigned exp = negative ? -e : e;

    long double result = 10.0;
    bool foundOne = false;
    for (int bit = 31; bit >= 0; bit--) {
        if (!foundOne) {
            if ((exp >> bit) & 1) {
                foundOne = true;
            }
        } else {
            result = result * result;
            if ((exp >> bit) & 1) {
                result = result * 10.0;
            }
        }
    }

    if (negative) {
        return static_cast<double>(1.0 / result);
    }
    return static_cast<double>(result);
}

static JSValue *numberToString(ExecState *exec, JSValue *v, const List &args)
{
    double radixAsDouble = args[0]->toInteger(exec); // nan -> 0
    if (radixAsDouble == 10 || args[0]->isUndefined()) {
        return jsString(v->toString(exec));
    }

    if (radixAsDouble < 2 || radixAsDouble > 36) {
        return throwError(exec, RangeError, "toString() radix argument must be between 2 and 36");
    }

    int radix = static_cast<int>(radixAsDouble);
    const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    // INT_MAX results in 1024 characters left of the dot with radix 2
    // give the same space on the right side. safety checks are in place
    // unless someone finds a precise rule.
    char s[2048 + 3];
    const char *lastCharInString = s + sizeof(s) - 1;
    double x = v->toNumber(exec);
    if (isNaN(x) || isInf(x)) {
        return jsString(UString::from(x));
    }

    bool isNegative = x < 0.0;
    if (isNegative) {
        x = -x;
    }

    double integerPart = floor(x);
    char *decimalPoint = s + sizeof(s) / 2;

    // convert integer portion
    char *p = decimalPoint;
    double d = integerPart;
    do {
        int remainderDigit = static_cast<int>(fmod(d, radix));
        *--p = digits[remainderDigit];
        d /= radix;
    } while ((d <= -1.0 || d >= 1.0) && s < p);

    if (isNegative) {
        *--p = '-';
    }
    char *startOfResultString = p;
    ASSERT(s <= startOfResultString);

    d = x - integerPart;
    p = decimalPoint;
    const double epsilon = 0.001; // TODO: guessed. base on radix ?
    bool hasFractionalPart = (d < -epsilon || d > epsilon);
    if (hasFractionalPart) {
        *p++ = '.';
        do {
            d *= radix;
            const int digit = static_cast<int>(d);
            *p++ = digits[digit];
            d -= digit;
        } while ((d < -epsilon || d > epsilon) && p < lastCharInString);
    }
    *p = '\0';
    ASSERT(p < s + sizeof(s));

    return jsString(startOfResultString);
}

static JSValue *numberToFixed(ExecState *exec, JSValue *v, const List &args)
{
    JSValue *fractionDigits = args[0];
    double df = fractionDigits->toInteger(exec);
    if (!(df >= 0 && df <= 20)) {
        return throwError(exec, RangeError, "toFixed() digits argument must be between 0 and 20");
    }
    int f = (int)df;

    double x = v->toNumber(exec);
    if (isNaN(x)) {
        return jsString("NaN");
    }

    UString s;
    if (x < 0) {
        s.append('-');
        x = -x;
    } else if (x == -0.0) {
        x = 0;
    }

    if (x >= pow(10.0, 21.0)) {
        return jsString(s + UString::from(x));
    }

    const double tenToTheF = pow(10.0, f);
    double n = floor(x * tenToTheF);
    if (fabs(n / tenToTheF - x) >= fabs((n + 1) / tenToTheF - x)) {
        n++;
    }

    UString m = integer_part_noexp(n);

    int k = m.size();
    if (k <= f) {
        UString z;
        for (int i = 0; i < f + 1 - k; i++) {
            z.append('0');
        }
        m = z + m;
        k = f + 1;
        ASSERT(k == m.size());
    }
    int kMinusf = k - f;
    if (kMinusf < m.size()) {
        return jsString(s + m.substr(0, kMinusf) + "." + m.substr(kMinusf));
    }
    return jsString(s + m.substr(0, kMinusf));
}

void fractionalPartToString(char *buf, int &i, const char *result, int resultLength, int fractionalDigits)
{
    if (fractionalDigits <= 0) {
        return;
    }

    int fDigitsInResult = static_cast<int>(resultLength) - 1;
    buf[i++] = '.';
    if (fDigitsInResult > 0) {
        if (fractionalDigits < fDigitsInResult) {
            strncpy(buf + i, result + 1, fractionalDigits);
            i += fractionalDigits;
        } else {
            strcpy(buf + i, result + 1);
            i += static_cast<int>(resultLength) - 1;
        }
    }

    for (int j = 0; j < fractionalDigits - fDigitsInResult; j++) {
        buf[i++] = '0';
    }
}

void exponentialPartToString(char *buf, int &i, int decimalPoint)
{
    buf[i++] = 'e';
    buf[i++] = (decimalPoint >= 0) ? '+' : '-';
    // decimalPoint can't be more than 3 digits decimal given the
    // nature of float representation
    int exponential = decimalPoint - 1;
    if (exponential < 0) {
        exponential *= -1;
    }
    if (exponential >= 100) {
        buf[i++] = static_cast<char>('0' + exponential / 100);
    }
    if (exponential >= 10) {
        buf[i++] = static_cast<char>('0' + (exponential % 100) / 10);
    }
    buf[i++] = static_cast<char>('0' + exponential % 10);
}

static JSValue *numberToExponential(ExecState *exec, JSValue *v, const List &args)
{
    double x = v->toNumber(exec);

    if (isNaN(x) || isInf(x)) {
        return jsString(UString::from(x));
    }

    JSValue *fractionalDigitsValue = args[0];
    double df = fractionalDigitsValue->toInteger(exec);
    if (!(df >= 0 && df <= 20)) {
        return throwError(exec, RangeError, "toExponential() argument must between 0 and 20");
    }
    int fractionalDigits = (int)df;
    bool includeAllDigits = fractionalDigitsValue->isUndefined();

    int decimalAdjust = 0;
    if (x && !includeAllDigits) {
        double logx = floor(log10(fabs(x)));
        x /= pow(10.0, logx);
        const double tenToTheF = pow(10.0, fractionalDigits);
        double fx = floor(x * tenToTheF) / tenToTheF;
        double cx = ceil(x * tenToTheF) / tenToTheF;

        if (fabs(fx - x) < fabs(cx - x)) {
            x = fx;
        } else {
            x = cx;
        }

        decimalAdjust = static_cast<int>(logx);
    }

    if (isNaN(x)) {
        return jsString("NaN");
    }

    if (x == -0.0) { // (-0.0).toExponential() should print as 0 instead of -0
        x = 0;
    }

    int decimalPoint;
    int sign;
    char *result = kjs_dtoa(x, 0, 0, &decimalPoint, &sign, nullptr);
    size_t resultLength = strlen(result);
    decimalPoint += decimalAdjust;

    int i = 0;
    char buf[80]; // digit + '.' + fractionDigits (max 20) + 'e' + sign + exponent (max?)
    if (sign) {
        buf[i++] = '-';
    }

    if (decimalPoint == 999) { // ? 9999 is the magical "result is Inf or NaN" value.  what's 999??
        strcpy(buf + i, result);
    } else {
        buf[i++] = result[0];

        if (includeAllDigits) {
            fractionalDigits = static_cast<int>(resultLength) - 1;
        }

        fractionalPartToString(buf, i, result, resultLength, fractionalDigits);
        exponentialPartToString(buf, i, decimalPoint);
        buf[i++] = '\0';
    }
    ASSERT(i <= 80);

    kjs_freedtoa(result);

    return jsString(buf);
}

static JSValue *numberToPrecision(ExecState *exec, JSValue *v, const List &args)
{
    double doublePrecision = args[0]->toIntegerPreserveNaN(exec);
    double x = v->toNumber(exec);
    if (args[0]->isUndefined() || isNaN(x) || isInf(x)) {
        return jsString(v->toString(exec));
    }

    UString s;
    if (x < 0) {
        s = "-";
        x = -x;
    }

    if (!(doublePrecision >= 1 && doublePrecision <= 21)) { // true for NaN
        return throwError(exec, RangeError, "toPrecision() argument must be between 1 and 21");
    }
    int precision = (int)doublePrecision;

    int e = 0;
    UString m;
    if (x) {
        e = static_cast<int>(log10(x));
        double tens = intPow10(e - precision + 1);
        double n = floor(x / tens);
        if (n < intPow10(precision - 1)) {
            e = e - 1;
            tens = intPow10(e - precision + 1);
            n = floor(x / tens);
        }

        if (fabs((n + 1.0) * tens - x) <= fabs(n * tens - x)) {
            ++n;
        }
        // maintain n < 10^(precision)
        if (n >= intPow10(precision)) {
            n /= 10.0;
            e += 1;
        }
        ASSERT(intPow10(precision - 1) <= n);
        ASSERT(n < intPow10(precision));

        m = integer_part_noexp(n);
        if (e < -6 || e >= precision) {
            if (m.size() > 1) {
                m = m.substr(0, 1) + "." + m.substr(1);
            }
            if (e >= 0) {
                return jsString(s + m + "e+" + UString::from(e));
            }
            return jsString(s + m + "e-" + UString::from(-e));
        }
    } else {
        m = char_sequence('0', precision);
        e = 0;
    }

    if (e == precision - 1) {
        return jsString(s + m);
    } else if (e >= 0) {
        if (e + 1 < m.size()) {
            return jsString(s + m.substr(0, e + 1) + "." + m.substr(e + 1));
        }
        return jsString(s + m);
    }
    return jsString(s + "0." + char_sequence('0', -(e + 1)) + m);
}

// ECMA 15.7.4.2 - 15.7.4.7
JSValue *NumberProtoFunc::callAsFunction(ExecState *exec, JSObject *thisObj, const List &args)
{
    // no generic function. "this" has to be a Number object
    if (!thisObj->inherits(&NumberInstance::info)) {
        return throwError(exec, TypeError);
    }

    JSValue *v = static_cast<NumberInstance *>(thisObj)->internalValue();
    switch (id) {
    case ToString:
        return numberToString(exec, v, args);
    case ToLocaleString: /* TODO */
        return jsString(v->toString(exec));
    case ValueOf:
        return jsNumber(v->toNumber(exec));
    case ToFixed:
        return numberToFixed(exec, v, args);
    case ToExponential:
        return numberToExponential(exec, v, args);
    case ToPrecision:
        return numberToPrecision(exec, v, args);
    }
    return nullptr;
}

// ------------------------------ NumberObjectImp ------------------------------

const ClassInfo NumberObjectImp::info = {"Function", &InternalFunctionImp::info, &numberTable, nullptr};

/* Source for number_object.lut.h
@begin numberTable 5
  NaN                   NumberObjectImp::NaNValue       DontEnum|DontDelete|ReadOnly
  NEGATIVE_INFINITY     NumberObjectImp::NegInfinity    DontEnum|DontDelete|ReadOnly
  POSITIVE_INFINITY     NumberObjectImp::PosInfinity    DontEnum|DontDelete|ReadOnly
  MAX_VALUE             NumberObjectImp::MaxValue       DontEnum|DontDelete|ReadOnly
  MIN_VALUE             NumberObjectImp::MinValue       DontEnum|DontDelete|ReadOnly

  MAX_SAFE_INTEGER      NumberObjectImp::MaxSafeInteger DontEnum|DontDelete|ReadOnly
  MIN_SAFE_INTEGER      NumberObjectImp::MinSafeInteger DontEnum|DontDelete|ReadOnly
  isFinite              NumberObjectImp::IsFinite       DontEnum|Function 1
  isInteger             NumberObjectImp::IsInteger      DontEnum|Function 1
  isNaN                 NumberObjectImp::IsNaN          DontEnum|Function 1
  isSafeInteger         NumberObjectImp::IsSafeInteger  DontEnum|Function 1
  parseInt              NumberObjectImp::ParseInt       DontEnum|Function 2
  parseFloat            NumberObjectImp::ParseFloat     DontEnum|Function 1
@end
*/
NumberObjectImp::NumberObjectImp(ExecState *exec, FunctionPrototype *funcProto, NumberPrototype *numberProto)
    : InternalFunctionImp(funcProto)
{
    // Number.Prototype
    putDirect(exec->propertyNames().prototype, numberProto, DontEnum | DontDelete | ReadOnly);

    // no. of arguments for constructor
    putDirect(exec->propertyNames().length, jsNumber(1), ReadOnly | DontDelete | DontEnum);
}

bool NumberObjectImp::getOwnPropertySlot(ExecState *exec, const Identifier &propertyName, PropertySlot &slot)
{
    return getStaticPropertySlot<NumberFuncImp, NumberObjectImp, InternalFunctionImp>(exec, &numberTable, this, propertyName, slot);
}

JSValue *NumberObjectImp::getValueProperty(ExecState *, int token) const
{
    // ECMA 15.7.3
    switch (token) {
    case NaNValue:
        return jsNaN();
    case NegInfinity:
        return jsNumberCell(-Inf);
    case PosInfinity:
        return jsNumberCell(Inf);
    case MaxValue:
        return jsNumberCell(1.7976931348623157E+308);
    case MinValue:
        return jsNumberCell(5E-324);
    case MaxSafeInteger:
        return jsNumber(MAX_SAFE_INTEGER);
    case MinSafeInteger:
        return jsNumber(MIN_SAFE_INTEGER);
    }
    return jsNull();
}

bool NumberObjectImp::implementsConstruct() const
{
    return true;
}

// ECMA 15.7.1
JSObject *NumberObjectImp::construct(ExecState *exec, const List &args)
{
    JSObject *proto = exec->lexicalInterpreter()->builtinNumberPrototype();
    NumberInstance *obj = new NumberInstance(proto);

    double n = args.isEmpty() ? 0 : args[0]->toNumber(exec);
    obj->setInternalValue(jsNumber(n));
    return obj;
}

// ECMA 15.7.2
JSValue *NumberObjectImp::callAsFunction(ExecState *exec, JSObject *, const List &args)
{
    double n = args.isEmpty() ? 0 : args[0]->toNumber(exec);
    return jsNumber(n);
}

NumberFuncImp::NumberFuncImp(ExecState* exec, int i, int l, const Identifier& name)
    : InternalFunctionImp(static_cast<FunctionPrototype*>(exec->lexicalInterpreter()->builtinFunctionPrototype()), name)
    , id(i)
{
    putDirect(exec->propertyNames().length, l, DontDelete|ReadOnly|DontEnum);
}

JSValue* NumberFuncImp::callAsFunction(ExecState* exec, JSObject* /*thisObj*/, const List& args)
{
    double arg = args[0]->toNumber(exec);

    switch (id) {
    case NumberObjectImp::IsFinite:
        if (args[0]->type() != NumberType)
            return jsBoolean(false);
        return jsBoolean(!isNaN(arg) && !isInf(arg));

    case NumberObjectImp::IsInteger:
    {
        if (args[0]->type() != NumberType)
            return jsBoolean(false);
        if (isNaN(arg) || isInf(arg))
            return jsBoolean(false);
        double num = args[0]->toInteger(exec);
        return jsBoolean(num == arg);
    }
    case NumberObjectImp::IsNaN:
        if (args[0]->type() != NumberType)
            return jsBoolean(false);
        return jsBoolean(isNaN(arg));

    case NumberObjectImp::IsSafeInteger:
    {
        if (args[0]->type() != NumberType)
            return jsBoolean(false);
        if (isNaN(arg) || isInf(arg))
            return jsBoolean(false);
        double num = args[0]->toInteger(exec);
        if (num != arg)
            return jsBoolean(false);
        return jsBoolean(fabs(num) <= MAX_SAFE_INTEGER);
    }
    case NumberObjectImp::ParseInt:
        return jsNumber(KJS::parseInt(args[0]->toString(exec), args[1]->toInt32(exec)));
    case NumberObjectImp::ParseFloat:
        return jsNumber(KJS::parseFloat(args[0]->toString(exec)));
    }
    return jsUndefined();
}

} // namespace KJS
