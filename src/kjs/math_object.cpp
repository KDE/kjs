/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "math_object.h"
#include "math_object.lut.h"
#include "wtf/MathExtras.h"

#include "operations.h"
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif  /*  M_PI  */

using namespace KJS;

// ------------------------------ MathObjectImp --------------------------------

const ClassInfo MathObjectImp::info = { "Math", nullptr, &mathTable, nullptr };

/* Source for math_object.lut.h
@begin mathTable 21
  E             MathObjectImp::Euler    DontEnum|DontDelete|ReadOnly
  LN2           MathObjectImp::Ln2      DontEnum|DontDelete|ReadOnly
  LN10          MathObjectImp::Ln10     DontEnum|DontDelete|ReadOnly
  LOG2E         MathObjectImp::Log2E    DontEnum|DontDelete|ReadOnly
  LOG10E        MathObjectImp::Log10E   DontEnum|DontDelete|ReadOnly
  PI            MathObjectImp::Pi       DontEnum|DontDelete|ReadOnly
  SQRT1_2       MathObjectImp::Sqrt1_2  DontEnum|DontDelete|ReadOnly
  SQRT2         MathObjectImp::Sqrt2    DontEnum|DontDelete|ReadOnly
  abs           MathObjectImp::Abs      DontEnum|Function 1
  acos          MathObjectImp::ACos     DontEnum|Function 1
  asin          MathObjectImp::ASin     DontEnum|Function 1
  atan          MathObjectImp::ATan     DontEnum|Function 1
  atan2         MathObjectImp::ATan2    DontEnum|Function 2
  ceil          MathObjectImp::Ceil     DontEnum|Function 1
  cos           MathObjectImp::Cos      DontEnum|Function 1
  exp           MathObjectImp::Exp      DontEnum|Function 1
  floor         MathObjectImp::Floor    DontEnum|Function 1
  log           MathObjectImp::Log      DontEnum|Function 1
  max           MathObjectImp::Max      DontEnum|Function 2
  min           MathObjectImp::Min      DontEnum|Function 2
  pow           MathObjectImp::Pow      DontEnum|Function 2
  random        MathObjectImp::Random   DontEnum|Function 0
  round         MathObjectImp::Round    DontEnum|Function 1
  sin           MathObjectImp::Sin      DontEnum|Function 1
  sqrt          MathObjectImp::Sqrt     DontEnum|Function 1
  tan           MathObjectImp::Tan      DontEnum|Function 1
  acosh         MathObjectImp::ACosH    DontEnum|Function 1
  acosh         MathObjectImp::ASinH    DontEnum|Function 1
  atanh         MathObjectImp::ATanH    DontEnum|Function 1
  cbrt          MathObjectImp::Cbrt     DontEnum|Function 1
  cosh          MathObjectImp::CosH     DontEnum|Function 1
  exmp1         MathObjectImp::Exmp1    DontEnum|Function 1
  log1p         MathObjectImp::Log1p    DontEnum|Function 1
  log10         MathObjectImp::Log10    DontEnum|Function 1
  log2          MathObjectImp::Log2     DontEnum|Function 1
  sign          MathObjectImp::Sign     DontEnum|Function 1
  sinh          MathObjectImp::SinH     DontEnum|Function 1
  tanh          MathObjectImp::TanH     DontEnum|Function 1
  trunc         MathObjectImp::Trunc    DontEnum|Function 1
  hypot         MathObjectImp::Hypot    DontEnum|Function 0
  imul          MathObjectImp::Imul     DontEnum|Function 2
  fround        MathObjectImp::FRound   DontEnum|Function 1
  clz32         MathObjectImp::Clz32    DontEnum|Function 1
@end
*/

MathObjectImp::MathObjectImp(ExecState * /*exec*/,
                             ObjectPrototype *objProto)
    : JSObject(objProto)
{
}

// ECMA 15.8

bool MathObjectImp::getOwnPropertySlot(ExecState *exec, const Identifier &propertyName, PropertySlot &slot)
{
    return getStaticPropertySlot<MathFuncImp, MathObjectImp, JSObject>(exec, &mathTable, this, propertyName, slot);
}

JSValue *MathObjectImp::getValueProperty(ExecState *, int token) const
{
    switch (token) {
    case Euler:
        return jsNumber(exp(1.0));
    case Ln2:
        return jsNumber(log(2.0));
    case Ln10:
        return jsNumber(log(10.0));
    case Log2E:
        return jsNumber(1.0 / log(2.0));
    case Log10E:
        return jsNumber(1.0 / log(10.0));
    case Pi:
        return jsNumber(piDouble);
    case Sqrt1_2:
        return jsNumber(sqrt(0.5));
    case Sqrt2:
        return jsNumber(sqrt(2.0));
    }

    assert(0);
    return nullptr;
}

// ------------------------------ MathObjectImp --------------------------------

static bool randomSeeded = false;

MathFuncImp::MathFuncImp(ExecState *exec, int i, int l, const Identifier &name)
    : InternalFunctionImp(static_cast<FunctionPrototype *>(exec->lexicalInterpreter()->builtinFunctionPrototype()), name)
    , id(i)
{
    putDirect(exec->propertyNames().length, l, DontDelete | ReadOnly | DontEnum);
}

JSValue *MathFuncImp::callAsFunction(ExecState *exec, JSObject * /*thisObj*/, const List &args)
{
    using namespace std;
    double arg = args[0]->toNumber(exec);
    double arg2 = args[1]->toNumber(exec);
    double result;

    switch (id) {
    case MathObjectImp::Abs:
        result = (arg < 0 || arg == -0) ? (-arg) : arg;
        break;
    case MathObjectImp::ACos:
        result = ::acos(arg);
        break;
    case MathObjectImp::ASin:
        result = ::asin(arg);
        break;
    case MathObjectImp::ATan:
        result = ::atan(arg);
        break;
    case MathObjectImp::ATan2:
        result = ::atan2(arg, arg2);
        break;
    case MathObjectImp::Ceil:
        result = ::ceil(arg);
        break;
    case MathObjectImp::Cos:
        result = ::cos(arg);
        break;
    case MathObjectImp::Exp:
        result = ::exp(arg);
        break;
    case MathObjectImp::Floor:
        result = ::floor(arg);
        break;
    case MathObjectImp::Log:
        result = ::log(arg);
        break;
    case MathObjectImp::Max: {
        unsigned int argsCount = args.size();
        result = -Inf;
        for (unsigned int k = 0; k < argsCount; ++k) {
            double val = args[k]->toNumber(exec);
            if (isNaN(val)) {
                result = NaN;
                break;
            }
            if (val > result || (val == 0 && result == 0 && !signbit(val))) {
                result = val;
            }
        }
        break;
    }
    case MathObjectImp::Min: {
        unsigned int argsCount = args.size();
        result = +Inf;
        for (unsigned int k = 0; k < argsCount; ++k) {
            double val = args[k]->toNumber(exec);
            if (isNaN(val)) {
                result = NaN;
                break;
            }
            if (val < result || (val == 0 && result == 0 && signbit(val))) {
                result = val;
            }
        }
        break;
    }
    case MathObjectImp::Pow:
        // ECMA 15.8.2.1.13 (::pow takes care of most of the critera)
        if (isNaN(arg2)) {
            result = NaN;
        } else if (isNaN(arg) && arg2 != 0) {
            result = NaN;
        } else if (::fabs(arg) == 1 && isInf(arg2)) {
            result = NaN;
        } else if (arg2 == 0 && arg != 0) {
            result = 1;
        } else {
            result = ::pow(arg, arg2);
        }
        break;
    case MathObjectImp::Random:
        if (!randomSeeded) {
            srand(static_cast<unsigned>(time(nullptr)));
            randomSeeded = true;
        }
        result = (double)rand() / RAND_MAX;
        break;
    case MathObjectImp::Round:
        if (signbit(arg) && arg >= -0.5) {
            result = -0.0;
        } else {
            result = ::floor(arg + 0.5);
        }
        break;
    case MathObjectImp::Sin:
        result = ::sin(arg);
        break;
    case MathObjectImp::Sqrt:
        result = ::sqrt(arg);
        break;
    case MathObjectImp::Tan:
        result = ::tan(arg);
        break;

    //ES6 (draft 08.11.2013)
    case MathObjectImp::ACosH:
        result = ::acosh(arg);
        break;
    case MathObjectImp::ASinH:
        result = ::asinh(arg);
        break;
    case MathObjectImp::ATanH:
        result = ::atanh(arg);
        break;
    case MathObjectImp::Cbrt:
        result = ::cbrt(arg);
        break;
    case MathObjectImp::CosH:
        result = ::cosh(arg);
        break;
    case MathObjectImp::Exmp1:
        result = ::expm1(arg);
        break;
    case MathObjectImp::Log1p:
        result = ::log1p(arg);
        break;
    case MathObjectImp::Log10:
        result = ::log10(arg);
        break;
    case MathObjectImp::Log2:
        result = ::log2(arg);
        break;
    case MathObjectImp::Sign:
        if (isNaN(arg))
        {
            result = KJS::NaN;
        }
        else if (signbit(arg))
        {
            if (arg == 0)
                result = -0.0;
            else
                result = -1.0;
        }
        else
        {
            if (arg == 0)
                result = 0.0;
            else
                result = 1.0;
        }
    break;
        case MathObjectImp::SinH:
        result = ::sinh(arg);
    break;
        case MathObjectImp::TanH:
        result = ::tanh(arg);
    break;
        case MathObjectImp::Trunc:
        result = ::trunc(arg);
    break;
    case MathObjectImp::Hypot:
    {
        if (args.size() == 0)
        {
            result = 0;
            break;
        }

        double sum = 0.0;
        bool foundNaN = false;
        for (int i = 0; i < args.size(); ++i)
        {
            double num = args[i]->toNumber(exec);
            if (isInf(num))
                return jsNumber(KJS::Inf);
            if (foundNaN)
                continue;
            if (isNaN(num))
            {
                foundNaN = true;
                continue;
            }

            sum += ::pow(num, 2);
        }

        if (foundNaN)
            return jsNumber(KJS::NaN);

        result = ::sqrt(sum);
        break;
    }
    case MathObjectImp::Imul:
    {
        if (args.size() < 2)
            return jsUndefined();
        int32_t a = args[0]->toInt32(exec);
        if (exec->hadException())
            return jsNumber(a);
        int32_t b = args[1]->toInt32(exec);
        if (exec->hadException())
            return jsNumber(b);

        result = a * b;
        break;
    }
    case MathObjectImp::FRound:
        if (isNaN(arg) || isInf(arg))
            return jsNumber(arg);

        result = static_cast<double>(static_cast<float>(arg));
        break;
    case MathObjectImp::Clz32:
    {
        uint32_t n = args[0]->toUInt32(exec);
        if (exec->hadException())
            return jsUndefined();
        return jsNumber(clz32(n));
    }
    default:
        result = 0.0;
        assert(0);
    }

    return jsNumber(result);
}
