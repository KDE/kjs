// -*- c-basic-offset: 2 -*-
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

#ifndef _KJS_REGEXP_H_
#define _KJS_REGEXP_H_

#include <sys/types.h>

#include "global.h"

#if HAVE_PCREPOSIX
#include <pcre.h>
#else  // POSIX regex - not so good...
extern "C" { // bug with some libc5 distributions
#include <regex.h>
}
#endif //HAVE_PCREPOSIX

#if defined _WIN32 || defined _WIN64
#undef HAVE_SYS_TIME_H
#endif
#if HAVE(SYS_TIME_H)
#include <sys/resource.h>
#endif

#include "ustring.h"

namespace KJS {


  // Represents a strign re-encoded to whatever PCRE can handle
  class RegExpStringContext {
  public:
    explicit RegExpStringContext(const UString& pattern);
    ~RegExpStringContext();

    char* buffer() const { return _buffer; }
    int   bufferSize() const { return _bufferSize; }
    int   originalPos(int c) const { return _originalPos[c]; }

  private:
    // Cached encoding info...
    char* _buffer;
    int*  _originalPos;
    int   _bufferSize;

    void prepareUtf8  (const UString& s);
    void prepareASCII (const UString& s);
#ifndef NDEBUG
  public:
    UString _originalS; // the original string, used for sanity-checking
#endif
  };

  class RegExp {
  public:
    enum { None = 0, Global = 1, IgnoreCase = 2, Multiline = 4 };

    explicit RegExp(const UString &pattern, char flags = None);
    ~RegExp();

    char flags() const { return _flags; }
    bool isValid() const { return _valid; }

    UString match(const RegExpStringContext& c, const UString& s, bool *error, int i, int *pos = 0, int **ovector = 0);
    unsigned subPatterns() const { return _numSubPatterns; }
    UString  pattern() const { return _pat; }

    static bool tryGrowingMaxStackSize;
    static bool didIncreaseMaxStackSize;
#if HAVE(SYS_TIME_H)
    static rlim_t availableStackSize;
#else
    static int availableStackSize;
#endif
  private:
#if HAVE_PCREPOSIX
    pcre *_regex;
#else
    regex_t _regex;
#endif
    UString _pat;
    char _flags;
    bool _valid;
    unsigned _numSubPatterns;
    
    RegExp(const RegExp &);
    RegExp &operator=(const RegExp &);

  public:
    enum UTF8SupportState {
      Unknown,
      Supported,
      Unsupported
    };

    static UTF8SupportState utf8Support;
  };

} // namespace

#endif
