/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2003 Apple Computer, Inc.
 *  Copyright (C) 2012 Bernd Buschinski (b.buschinski@googlemail.com)
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

#ifndef COMMONUNICODE_H_
#define COMMONUNICODE_H_

namespace KJS {
    namespace CommonUnicode {

    inline bool isLineTerminator(unsigned short c)
    {
        switch (c) {
            case 0x000A:  // LINE FEED
            case 0x000D:  // CARRIAGE RETURN
            case 0x2028:  // LINE SEPARATOR
            case 0x2029:  // PARAGRAPH SEPARATOR
                return true;
            default:
                return false;
        }
    }

    inline bool isWhiteSpace(unsigned short c)
    {
        switch (c) {
            case 0x0009:
            case 0x000B:
            case 0x000C:
            // Unicode category Zs
            case 0x0020:  // SPACE
            case 0x00A0:  // NO-BREAK SPACE
            case 0x1680:  // OGHAM SPACE MARK
            case 0x180E:  // MONGOLIAN VOWEL SEPARATOR
            case 0x2000:  // EN QUAD
            case 0x2001:  // EM QUAD
            case 0x2002:  // EN SPACE
            case 0x2003:  // EM SPACE
            case 0x2004:  // THREE-PER-EM SPACE
            case 0x2005:  // FOUR-PER-EM SPACE
            case 0x2006:  // SIX-PER-EM SPACE
            case 0x2007:  // FIGURE SPACE
            case 0x2008:  // PUNCTUATION SPACE
            case 0x2009:  // THIN SPACE
            case 0x200A:  // HAIR SPACE
            case 0x202F:  // NARROW NO-BREAK SPACE
            case 0x205F:  // MEDIUM MATHEMATICAL SPACE
            case 0x3000:  // IDEOGRAPHIC SPACE
            // Unicode Byte-Order-Mark, Ecmascript 5.1r6 - 7.2
            case 0xFEFF:  // ZERO WIDTH NO-BREAK SPACE, BOM
                return true;
            default:
                return false;
        }
    }

    inline bool isStrWhiteSpace(unsigned short c)
    {
        return isWhiteSpace(c) || isLineTerminator(c);
    }

    } //namespace CommonUnicode
} //namespace KJS

#endif //COMMONUNICODE_H_
