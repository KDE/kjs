// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2007 Harri Porten <porten@kde.org>
 *  Copyright (C) 2006 George Staikos <staikos@kde.org>
 *  Copyright (C) 2006 Alexey Proskuryakov <ap@nypop.com>
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

#ifndef KJS_UNICODE_LIBC_H
#define KJS_UNICODE_LIBC_H

#include "wtf/ASCIICType.h"
#include <assert.h>

#include "../UnicodeCategory.h"

namespace WTF {
  namespace Unicode {

    inline int toLower(uint16_t* str, int strLength, uint16_t*& destIfNeeded)
    {
      destIfNeeded = 0;

      for (int i = 0; i < strLength; ++i)
        str[i] = toASCIILower(str[i]);

      return strLength;
    }

    inline int toUpper(uint16_t* str, int strLength, uint16_t*& destIfNeeded)
    {
      destIfNeeded = 0;

      for (int i = 0; i < strLength; ++i)
        str[i] = toASCIIUpper(str[i]);

      return strLength;
    }

    inline bool isSeparatorSpace(int32_t c)
    {
      return (c & 0xffff0000) == 0 && isASCIISpace(static_cast<unsigned short>(c));
    }

    inline CharCategory category(int32_t c)
    {
      if (c < 0)
        return NoCategory;
      if (c < 0x000000ff) {
          static const CharCategory cats[] = {
          Other_Control, Other_Control, Other_Control, Other_Control,
          Other_Control, Other_Control, Other_Control, Other_Control,
          Other_Control, Other_Control, Other_Control, Other_Control,
          Other_Control, Other_Control, Other_Control, Other_Control,
          Other_Control, Other_Control, Other_Control, Other_Control,
          Other_Control, Other_Control, Other_Control, Other_Control,
          Other_Control, Other_Control, Other_Control, Other_Control,
          Other_Control, Other_Control, Other_Control, Other_Control,
          Separator_Space, Punctuation_Other, Punctuation_Other,
          Punctuation_Other, Symbol_Currency, Punctuation_Other,
          Punctuation_Other, Punctuation_Other, Punctuation_Open,
          Punctuation_Close, Punctuation_Other, Symbol_Math,
          Punctuation_Other, Punctuation_Dash, Punctuation_Other,
          Punctuation_Other, Number_DecimalDigit, Number_DecimalDigit,
          Number_DecimalDigit, Number_DecimalDigit, Number_DecimalDigit,
          Number_DecimalDigit, Number_DecimalDigit, Number_DecimalDigit,
          Number_DecimalDigit, Number_DecimalDigit, Punctuation_Other,
          Punctuation_Other, Symbol_Math, Symbol_Math, Symbol_Math,
          Punctuation_Other, Punctuation_Other,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Punctuation_Open,
          Punctuation_Other, Punctuation_Close, Symbol_Modifier,
          Punctuation_Connector, Symbol_Modifier, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Punctuation_Open, Symbol_Math, Punctuation_Close,
          Symbol_Math, Other_Control, Other_Control, Other_Control,
          Other_Control, Other_Control,Other_Control, Other_Control,
          Other_Control, Other_Control, Other_Control, Other_Control,
          Other_Control, Other_Control, Other_Control, Other_Control,
          Other_Control, Other_Control, Other_Control, Other_Control,
          Other_Control, Other_Control, Other_Control, Other_Control,
          Other_Control, Other_Control, Other_Control, Other_Control,
          Other_Control, Other_Control, Other_Control, Other_Control,
          Other_Control, Other_Control, Separator_Space, Punctuation_Other,
          Symbol_Currency, Symbol_Currency, Symbol_Currency, Symbol_Currency,
          Symbol_Other, Symbol_Other, Symbol_Modifier, Symbol_Other,
          Letter_Lowercase, Punctuation_InitialQuote, Symbol_Math,
          Other_Format, Symbol_Other, Symbol_Modifier, Symbol_Other,
          Symbol_Math, Number_Other, Number_Other, Symbol_Modifier,
          Letter_Lowercase, Symbol_Other, Punctuation_Other,
          Symbol_Modifier, Number_Other, Letter_Lowercase,
          Punctuation_FinalQuote, Number_Other, Number_Other,
          Number_Other, Punctuation_Other, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Symbol_Math, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Uppercase, Letter_Uppercase, Letter_Uppercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Symbol_Math, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase,
          Letter_Lowercase, Letter_Lowercase, Letter_Lowercase
        };
        assert(sizeof(cats)/sizeof(CharCategory) == 0x0100);
        return cats[c];
      }

      // FIXME: implement for the rest ...
      return NoCategory;
    }
  }
}

#endif
// vim: ts=2 sw=2 et
