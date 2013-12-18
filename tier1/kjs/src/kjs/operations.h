// -*- c-basic-offset: 2 -*-
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

namespace KJS {

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

#if PLATFORM(DARWIN)
  inline bool isNaN(double d) { return isnan(d); }
  inline bool isInf(double d) { return isinf(d); }
  inline bool isPosInf(double d) { return isinf(d) && d > 0; }
  inline bool isNegInf(double d) { return isinf(d) && d < 0; }
#else
  KJS_EXPORT bool isNaN(double d);
  KJS_EXPORT bool isInf(double d);
  KJS_EXPORT bool isPosInf(double d);
  KJS_EXPORT bool isNegInf(double d);
#endif

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
