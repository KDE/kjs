/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
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

#ifndef _KJS_OPERATIONS_H_
#define _KJS_OPERATIONS_H_

#include "global.h"
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <wtf/MathExtras.h>

#if HAVE(IEEEFP_H)
#   if HAVE(FUNC_ISINF) || HAVE(FUNC_FINITE)
#       include <ieeefp.h>
#   endif
#endif

#if HAVE(FLOAT_H)
#   include <float.h>
#endif

namespace KJS
{

class ExecState;
class JSValue;

enum Operator { OpEqual,
                OpEqEq,
                OpPlus,
                OpMinus,
                OpMult,
                OpDiv,
                OpMod,
                OpNotEq,
                OpStrEq,
                OpStrNEq,
                OpPlusEq,
                OpMinusEq,
                OpMultEq,
                OpDivEq,
                OpPlusPlus,
                OpMinusMinus,
                OpLess,
                OpLessEq,
                OpGreater,
                OpGreaterEq,
                OpAndEq,
                OpXOrEq,
                OpOrEq,
                OpModEq,
                OpAnd,
                OpOr,
                OpBitAnd,
                OpBitXOr,
                OpBitOr,
                OpLShift,
                OpRShift,
                OpURShift,
                OpIn,
                OpInstanceOf
              };

inline bool isNaN(double d);
inline bool isFinite(double d);
inline bool isInf(double d);
inline double signBit(double d);
inline bool isPosInf(double d);
inline bool isNegInf(double d);

inline int clz32(unsigned int d)
{
#if HAVE(FUNC_BUILTIN_CLZ)
    //NOTE: __builtin_clz(0); is undefined
    //buschinski: and returns 1 for me, for whatever reason
    if (d == 0)
        return 32;
    return __builtin_clz(d);
#else
    int count = 0;
    while (d != 0)
    {
        ++count;
        d >>= 1;
    }
    return 32 - count;
#endif
}

inline bool isNaN(double d)
{
#if HAVE(FUNC_STD_ISNAN)
    return std::isnan(d);
#elif HAVE(FUNC_ISNAN)
    return isnan(d) != 0;
#elif HAVE(FLOAT_H)
    return _isnan(d) != 0;
#else
    return !(d == d);
#endif
}

inline bool isFinite(double d)
{
#if HAVE(FUNC_STD_ISFINITE)
    return std::isfinite(d) != 0;
#elif HAVE(FUNC_FINITE)
    return finite(d) != 0;
#elif HAVE(FUNC__FINITE)
    return _finite(d) != 0;
#else
    return !isNaN(d) && !isInf(d);
#endif
}

inline bool isInf(double d)
{
#if HAVE(FUNC_STD_ISINF)
    return std::isinf(d);
#elif HAVE(FUNC_ISINF)
    return isinf(d);
#elif HAVE(FUNC_FINITE)
    return finite(d) == 0 && d == d;
#elif HAVE(FUNC__FINITE)
    return _finite(d) == 0 && d == d;
#elif HAVE(FUNC__FPCLASS)
    int fpClass = _fpclass(d);
    return _FPCLASS_PINF == fpClass || _FPCLASS_NINF == fpClass;
#else
# error "Your platform does not provide a Infinity checking function."
    return false;
#endif
}

inline double signBit(double d)
{
#if HAVE(FUNC_STD_SIGNBIT)
    return std::signbit(d);
#elif HAVE(FUNC_SIGNBIT)
    return signbit(d);
#elif HAVE(FUNC___SIGNBIT)
    return __signbit(d);
#elif HAVE(FUNC__COPYSIGN)
    return _copysign(1.0, d) < 0 ? 1.0 : 0.0;
#elif HAVE(FUNC_COPYSIGN)
    return copysign(1.0, d) < 0 ? 1.0 : 0.0;
#else
# error "Your platform does not provide a signbit/copysign function."
    return false;
#endif
}

inline bool isPosInf(double d)
{
#if HAVE(FPCLASS)
    return _FPCLASS_PINF == _fpclass(d);
#else
    return isInf(d) && !signBit(d);
#endif
}

inline bool isNegInf(double d)
{
#if HAVE(FUNC_FPCLASS)
    return _FPCLASS_PINF == _fpclass(d);
#else
    return isInf(d) && signBit(d);
#endif
}

bool equal(ExecState *exec, JSValue *v1, JSValue *v2);
bool strictEqual(ExecState *exec, JSValue *v1, JSValue *v2);
// Same as strictEqual, except for "-0" and "0" difference.
// Used in PropertyDescriptor::equalTo and Object::defineOwnProperty to check
// if the value changed and we need to update.
bool sameValue(ExecState *exec, JSValue *v1, JSValue *v2);
/**
 * This operator performs an abstract relational comparison of the two
 * arguments that can be of arbitrary type. If possible, conversions to the
 * string or number type will take place before the comparison.
 *
 * @return 1 if v1 is "less-than" v2, 0 if the relation is "greater-than-or-
 * equal". -1 if the result is undefined.
 */
int relation(ExecState *exec, JSValue *v1, JSValue *v2, bool leftFirst = true);
int relation(ExecState *exec, JSValue *v1, double n2);
int maxInt(int d1, int d2);
int minInt(int d1, int d2);

}

#endif
