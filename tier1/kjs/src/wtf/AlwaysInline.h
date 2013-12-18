/*
 *  Copyright (C) 2005, 2007 Apple Inc. All rights reserved.
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

#include <wtf/Platform.h>


#ifndef ALWAYS_INLINE
#if COMPILER(GCC) && defined(NDEBUG) &&  __GNUC__ > 3
#define ALWAYS_INLINE inline __attribute__ ((__always_inline__))
#elif COMPILER(MSVC) && defined(NDEBUG)
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE inline
#endif
#endif

#ifndef ALWAYS_INLINE_INTO
#if COMPILER(GCC) && defined(NDEBUG) &&  ((__GNUC__ == 4 && __GNUC_MINOR__ >= 1) || __GNUC__ > 4)
#define ALWAYS_INLINE_INTO __attribute__ ((__flatten__))
#else
#define ALWAYS_INLINE_INTO
#endif
#endif


#ifndef NEVER_INLINE
#if COMPILER(GCC) &&  __GNUC__ > 3
#define NEVER_INLINE __attribute__ ((__noinline__))
#else
#define NEVER_INLINE
#endif
#endif
