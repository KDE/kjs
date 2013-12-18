/* -*- mode: c++; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef WTF_Platform_h
#define WTF_Platform_h

/* Force KDE build here in our tree... */
#ifndef BUILDING_KDE__
#define BUILDING_KDE__ 1
#endif

/* PLATFORM handles OS, operating environment, graphics API, and CPU */
#define PLATFORM(WTF_FEATURE) (defined(WTF_PLATFORM_##WTF_FEATURE) && WTF_PLATFORM_##WTF_FEATURE)
#define COMPILER(WTF_FEATURE) (defined(WTF_COMPILER_##WTF_FEATURE) && WTF_COMPILER_##WTF_FEATURE)
#define HAVE(WTF_FEATURE) (defined(HAVE_##WTF_FEATURE) && HAVE_##WTF_FEATURE)
#define USE(WTF_FEATURE) (defined(WTF_USE_##WTF_FEATURE) && WTF_USE_##WTF_FEATURE)
#define ENABLE(WTF_FEATURE) (defined(ENABLE_##WTF_FEATURE) && ENABLE_##WTF_FEATURE)

/* Operating systems - low-level dependencies */

/* PLATFORM(DARWIN) */
/* Operating system level dependencies for Mac OS X / Darwin that should */
/* be used regardless of operating environment */
#ifdef __APPLE__
#define WTF_PLATFORM_DARWIN 1
#endif

/* PLATFORM(WIN_OS) */
/* Operating system level dependencies for Windows that should be used */
/* regardless of operating environment */
#if defined(WIN32) || defined(_WIN32)
#define WTF_PLATFORM_WIN_OS 1
#endif

/* PLATFORM(UNIX) */
/* Operating system level dependencies for Unix-like systems that */
/* should be used regardless of operating environment */
/* (includes PLATFORM(DARWIN)) */
#if   defined(__APPLE__)   \
   || defined(unix)        \
   || defined(__unix)      \
   || defined(__unix__)    \
   || defined (__NetBSD__) \
   || defined(_AIX)
#define WTF_PLATFORM_UNIX 1
#endif

/* PLATFORM(SOLARIS_OS) */
/* Operating system level dependencies for Sun (Open)Solaris 10. */
/* Studio 12 on Solaris defines __SunOS; gcc defines __sun__; */
/* Both compilers define __sun and sun. */
#if defined(__sun) || defined(sun)
#define WTF_PLATFORM_SOLARIS_OS 1
#endif

/* Operating environments */

/* I made the BUILDING_KDE__ macro up for the KDE build system to define */

/* PLATFORM(KDE) */
/* PLATFORM(MAC) */
/* PLATFORM(WIN) */
#if BUILDING_KDE__
#define WTF_PLATFORM_KDE 1
#elif PLATFORM(DARWIN)
#define WTF_PLATFORM_MAC 1
#elif PLATFORM(WIN_OS)
#define WTF_PLATFORM_WIN 1
#endif
#if defined(BUILDING_GDK__)
#define WTF_PLATFORM_GDK 1
#endif


/* CPU */

/* PLATFORM(PPC) */
#if   defined(__ppc__)     \
   || defined(__PPC__)     \
   || defined(__powerpc__) \
   || defined(__powerpc)   \
   || defined(__POWERPC__) \
   || defined(_M_PPC)      \
   || defined(__PPC)
#define WTF_PLATFORM_PPC 1
#define WTF_PLATFORM_BIG_ENDIAN 1
#endif

/* PLATFORM(PPC64) */
#if   defined(__ppc64__) \
   || defined(__PPC64__)
#define WTF_PLATFORM_PPC64 1
#define WTF_PLATFORM_BIG_ENDIAN 1
#endif

#if defined(arm)
#define WTF_PLATFORM_ARM 1
#if defined(__ARMEB__)
#define WTF_PLATFORM_BIG_ENDIAN 1
#elif !defined(__ARM_EABI__) && !defined(__ARMEB__)
#define WTF_PLATFORM_MIDDLE_ENDIAN 1
#endif
#if !defined(__ARM_EABI__)
#define WTF_PLATFORM_FORCE_PACK 1
#endif
#endif

/* PLATFORM(X86) */
#if   defined(__i386__) \
   || defined(i386)     \
   || defined(_M_IX86)  \
   || defined(_X86_)    \
   || defined(__THW_INTEL)
#define WTF_PLATFORM_X86 1
#endif

/* PLATFORM(X86_64) */
#if   defined(__x86_64__) \
   || defined(__ia64__)
#define WTF_PLATFORM_X86_64 1
#endif

/* PLATFORM(SPARC) */
#if defined(sparc)
#define WTF_PLATFORM_SPARC 1
#endif

/* Compiler */

/* COMPILER(CWP) */
#if defined(__MWERKS__)
#define WTF_COMPILER_CWP 1
#endif

/* COMPILER(MSVC) */
#if defined(_MSC_VER)
#define WTF_COMPILER_MSVC 1
#endif

/* COMPILER(GCC) */
#if defined(__GNUC__)
#define WTF_COMPILER_GCC 1
#endif

/* COMPILER(SUNPRO) */
#if defined(__SUNPRO_CC)
#define WTF_COMPILER_SUNPRO 1
#endif

/* COMPILER(BORLAND) */
/* not really fully supported - is this relevant any more? */
#if defined(__BORLANDC__)
#define WTF_COMPILER_BORLAND 1
#endif

/* COMPILER(CYGWIN) */
/* not really fully supported - is this relevant any more? */
#if defined(__CYGWIN__)
#define WTF_COMPILER_CYGWIN 1
#endif

/* multiple threads only supported on Mac for now */
#if PLATFORM(MAC)
#ifndef WTF_USE_MULTIPLE_THREADS
#define WTF_USE_MULTIPLE_THREADS 1
#endif
#ifndef WTF_USE_BINDINGS
#define WTF_USE_BINDINGS 1
#endif
#endif

/* for Unicode, KDE uses Qt, everything else uses ICU */
#if PLATFORM(KDE) || PLATFORM(QT)
#define WTF_USE_QT4_UNICODE 1
#elif PLATFORM(SYMBIAN)
#define WTF_USE_SYMBIAN_UNICODE 1
#else
#define WTF_USE_ICU_UNICODE 1
#endif

#if PLATFORM(MAC)
#define WTF_PLATFORM_CF 1
#endif

#if PLATFORM(WIN)
#define WTF_USE_WININET 1
#endif

#if PLATFORM(GDK)
#define WTF_USE_CURL 1
#endif

/* ENABLE macro defaults */

#endif /* WTF_Platform_h */
