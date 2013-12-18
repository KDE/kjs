/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2004 Apple Computer, Inc.
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

#include "date_object.h"
#include "global.h"
#include "date_object.lut.h"
#include "internal.h"

#if HAVE_ERRNO_H
#include <errno.h>
#endif

#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#if HAVE_SYS_TIMEB_H
#include <sys/timeb.h>
#endif

#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <time.h>

#if PLATFORM(SOLARIS_OS)
#include <strings.h>
#endif

#include "error_object.h"
#include "operations.h"

#if PLATFORM(MAC)
#include <CoreFoundation/CoreFoundation.h>
#endif

#if PLATFORM(WIN_OS)
#include <windows.h>
#define copysign(x, y) _copysign(x, y)
#define snprintf _snprintf
#if !COMPILER(GCC)
#define isfinite(x) _finite(x)
#ifndef strncasecmp
#define strncasecmp(x, y, z) strnicmp(x, y, z)
#endif
#endif
#endif

#include "wtf/DisallowCType.h"
#include "wtf/ASCIICType.h"

// GCC cstring uses these automatically, but not all implementations do.
using std::strlen;
using std::strcpy;
using std::strncpy;
using std::memset;
using std::memcpy;

using namespace WTF;


inline int gmtoffset(const tm& t)
{
#if PLATFORM(WIN_OS)
    // Time is supposed to be in the current timezone.
    // FIXME: Use undocumented _dstbias?
    return -(_timezone / 60 - (t.tm_isdst > 0 ? 60 : 0 )) * 60;
#else
#if HAVE_TM_GMTOFF
    return t.tm_gmtoff;
#else
    return - timezone;
#endif
#endif
}

namespace KJS {

/**
 * @internal
 *
 * Class to implement all methods that are properties of the
 * Date object
 */
class DateObjectFuncImp : public InternalFunctionImp {
public:
    DateObjectFuncImp(ExecState *, FunctionPrototype *, int i, int len, const Identifier& );

    virtual JSValue *callAsFunction(ExecState *, JSObject *thisObj, const List &args);

    enum { Parse, UTC, Now };

private:
    int id;
};

// some constants
const double hoursPerDay = 24;
const double minutesPerHour = 60;
const double secondsPerMinute = 60;
const double msPerSecond = 1000;
const double msPerMinute = 60 * 1000;
const double msPerHour = 60 * 60 * 1000;
const double msPerDay = 24 * 60 * 60 * 1000;

static const char * const weekdayName[7] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
static const char * const monthName[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

static double makeTime(tm *, double ms, bool utc);
static double parseDate(const UString &);
static double timeClip(double);
static void millisecondsToTM(double milli, bool utc, tm *t);

#if PLATFORM(MAC)

static CFDateFormatterStyle styleFromArgString(const UString& string, CFDateFormatterStyle defaultStyle)
{
    if (string == "short")
        return kCFDateFormatterShortStyle;
    if (string == "medium")
        return kCFDateFormatterMediumStyle;
    if (string == "long")
        return kCFDateFormatterLongStyle;
    if (string == "full")
        return kCFDateFormatterFullStyle;
    return defaultStyle;
}

static UString formatLocaleDate(ExecState *exec, double time, bool includeDate, bool includeTime, const List &args)
{
    CFDateFormatterStyle dateStyle = (includeDate ? kCFDateFormatterLongStyle : kCFDateFormatterNoStyle);
    CFDateFormatterStyle timeStyle = (includeTime ? kCFDateFormatterLongStyle : kCFDateFormatterNoStyle);

    bool useCustomFormat = false;
    UString customFormatString;

    JSValue* arg0 = args[0];
    JSValue* arg1 = args[1];
    UString arg0String = arg0->toString(exec);
    if (arg0String == "custom" && !arg1->isUndefined()) {
        useCustomFormat = true;
        customFormatString = arg1->toString(exec);
    } else if (includeDate && includeTime && !arg1->isUndefined()) {
        dateStyle = styleFromArgString(arg0String, dateStyle);
        timeStyle = styleFromArgString(arg1->toString(exec), timeStyle);
    } else if (includeDate && !arg0->isUndefined()) {
        dateStyle = styleFromArgString(arg0String, dateStyle);
    } else if (includeTime && !arg0->isUndefined()) {
        timeStyle = styleFromArgString(arg0String, timeStyle);
    }

    CFLocaleRef locale = CFLocaleCopyCurrent();
    CFDateFormatterRef formatter = CFDateFormatterCreate(0, locale, dateStyle, timeStyle);
    CFRelease(locale);

    if (useCustomFormat) {
        CFStringRef customFormatCFString = CFStringCreateWithCharacters(0, (UniChar *)customFormatString.data(), customFormatString.size());
        CFDateFormatterSetFormat(formatter, customFormatCFString);
        CFRelease(customFormatCFString);
    }

    CFStringRef string = CFDateFormatterCreateStringWithAbsoluteTime(0, formatter, time - kCFAbsoluteTimeIntervalSince1970);

    CFRelease(formatter);

    // We truncate the string returned from CFDateFormatter if it's absurdly long (> 200 characters).
    // That's not great error handling, but it just won't happen so it doesn't matter.
    UChar buffer[200];
    const size_t bufferLength = sizeof(buffer) / sizeof(buffer[0]);
    size_t length = CFStringGetLength(string);
    assert(length <= bufferLength);
    if (length > bufferLength)
        length = bufferLength;
    CFStringGetCharacters(string, CFRangeMake(0, length), reinterpret_cast<UniChar *>(buffer));

    CFRelease(string);

    return UString(buffer, length);
}

#endif // PLATFORM(MAC)

static UString formatDate(const tm &t)
{
    char buffer[100];
    int len = snprintf(buffer, sizeof(buffer), "%s %s %02d %04d",
        weekdayName[(t.tm_wday + 6) % 7],
        monthName[t.tm_mon], t.tm_mday, t.tm_year + 1900);
    return UString(buffer, len);
}

static UString formatDateUTCVariant(const tm &t)
{
    char buffer[100];
    int len = snprintf(buffer, sizeof(buffer), "%s, %02d %s %04d",
        weekdayName[(t.tm_wday + 6) % 7],
        t.tm_mday, monthName[t.tm_mon], t.tm_year + 1900);
    return UString(buffer, len);
}

static UString formatDateISOVariant(const tm &t, bool utc, double absoluteMS)
{
    char buffer[100];
    // YYYY-MM-DD
    int len;
    if (utc) {
        len = snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d",
            t.tm_year + 1900, t.tm_mon+1, t.tm_mday);
    } else {
        int offset = gmtoffset(t);
        tm t_fixed;
        millisecondsToTM(absoluteMS - offset*1000, true, &t_fixed);
        len = snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d",
            t_fixed.tm_year + 1900, t_fixed.tm_mon+1, t_fixed.tm_mday);
    }
    return UString(buffer, len);
}

static UString formatTime(const tm &t, bool utc)
{
    char buffer[100];
    int len;
    if (utc) {
        // FIXME: why not on windows?
#if !PLATFORM(WIN_OS)
        ASSERT(gmtoffset(t) == 0);
#endif
        len = snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d GMT", t.tm_hour, t.tm_min, t.tm_sec);
    } else {
        int offset = abs(gmtoffset(t));
        len = snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d GMT%c%02d%02d",
            t.tm_hour, t.tm_min, t.tm_sec,
            gmtoffset(t) < 0 ? '-' : '+', offset / (60*60), (offset / 60) % 60);
    }
    return UString(buffer, len);
}

static UString formatTimeISOVariant(const tm &t, bool utc, double absoluteMS, double ms)
{
    char buffer[100];
    // HH:mm:ss.sss
    int len;
    if (utc) {
        len = snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d.%03d",
                       t.tm_hour, t.tm_min, t.tm_sec, int(ms));
    } else {
        int offset = gmtoffset(t);
        tm t_fixed;
        millisecondsToTM(absoluteMS - offset*1000, true, &t_fixed);
        len = snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d.%03d",
            t_fixed.tm_hour, t_fixed.tm_min, t_fixed.tm_sec, int(ms));
    }
    return UString(buffer, len);
}

static int day(double t)
{
    return int(floor(t / msPerDay));
}

static double dayFromYear(int year)
{
    return 365.0 * (year - 1970)
        + floor((year - 1969) / 4.0)
        - floor((year - 1901) / 100.0)
        + floor((year - 1601) / 400.0);
}

// based on the rule for whether it's a leap year or not
static int daysInYear(int year)
{
    if (year % 4 != 0)
        return 365;
    if (year % 400 == 0)
        return 366;
    if (year % 100 == 0)
        return 365;
    return 366;
}

// time value of the start of a year
static double timeFromYear(int year)
{
    return msPerDay * dayFromYear(year);
}

// year determined by time value
static int yearFromTime(double t)
{
    // ### there must be an easier way

    // initial guess
    int y = 1970 + int(t / (365.25 * msPerDay));

    // adjustment
    if (timeFromYear(y) > t) {
        do
            --y;
        while (timeFromYear(y) > t);
    } else {
        while (timeFromYear(y + 1) < t)
            ++y;
    }

    return y;
}

// 0: Sunday, 1: Monday, etc.
static int weekDay(double t)
{
    int wd = (day(t) + 4) % 7;
    if (wd < 0)
        wd += 7;
    return wd;
}

// Converts a list of arguments sent to a Date member function into milliseconds, updating
// ms (representing milliseconds) and t (representing the rest of the date structure) appropriately.
//
// Format of member function: f([hour,] [min,] [sec,] [ms])
static double setTimeFields(ExecState* exec, const List& args, int id, double ms, tm* t)
{
    assert(DateProtoFunc::SetSeconds - DateProtoFunc::SetMilliSeconds + 1 == 2);
    assert(DateProtoFunc::SetMinutes - DateProtoFunc::SetMilliSeconds + 1 == 3);
    assert(DateProtoFunc::SetHours   - DateProtoFunc::SetMilliSeconds + 1 == 4);

    assert(id == DateProtoFunc::SetMilliSeconds || id == DateProtoFunc::SetSeconds ||
           id == DateProtoFunc::SetMinutes || id == DateProtoFunc::SetHours);

    int maxArgs = id - DateProtoFunc::SetMilliSeconds + 1;
    double milliseconds = 0;
    int idx = 0;
    int numArgs = args.size();

    // JS allows extra trailing arguments -- ignore them
    if (numArgs > maxArgs)
        numArgs = maxArgs;

    // hours
    if (maxArgs >= 4 && idx < numArgs) {
        t->tm_hour = 0;
        milliseconds += args[idx++]->toInt32(exec) * msPerHour;
    }

    // minutes
    if (maxArgs >= 3 && idx < numArgs) {
        t->tm_min = 0;
        milliseconds += args[idx++]->toInt32(exec) * msPerMinute;
    }

    // seconds
    if (maxArgs >= 2 && idx < numArgs) {
        t->tm_sec = 0;
        milliseconds += args[idx++]->toInt32(exec) * msPerSecond;
    }

    // milliseconds
    if (idx < numArgs) {
        milliseconds += roundValue(exec, args[idx]);
    } else {
        milliseconds += ms;
    }

    return milliseconds;
}

// Converts a list of arguments sent to a Date member function into years, months, and milliseconds, updating
// ms (representing milliseconds) and t (representing the rest of the date structure) appropriately.
//
// Format of member function: f([years,] [months,] [days])
static double setDateFields(ExecState* exec, const List& args, int id, double ms, tm* t)
{
    assert(DateProtoFunc::SetMonth    - DateProtoFunc::SetDate + 1 == 2);
    assert(DateProtoFunc::SetFullYear - DateProtoFunc::SetDate + 1 == 3);

    assert(id == DateProtoFunc::SetDate || id == DateProtoFunc::SetMonth || id == DateProtoFunc::SetFullYear);

    int maxArgs = id - DateProtoFunc::SetDate + 1;
    int idx = 0;
    int numArgs = args.size();

    // JS allows extra trailing arguments -- ignore them
    if (numArgs > maxArgs)
        numArgs = maxArgs;

    // years
    if (maxArgs >= 3 && idx < numArgs)
        t->tm_year = args[idx++]->toInt32(exec) - 1900;

    // months
    if (maxArgs >= 2 && idx < numArgs)
        t->tm_mon = args[idx++]->toInt32(exec);

    // days
    if (idx < numArgs) {
        t->tm_mday = 0;
        ms += args[idx]->toInt32(exec) * msPerDay;
    }

    return ms;
}

// ------------------------------ DateInstance ------------------------------

const ClassInfo DateInstance::info = {"Date", 0, 0, 0};

DateInstance::DateInstance(JSObject *proto)
  : JSWrapperObject(proto)
{
}

JSObject* DateInstance::valueClone(Interpreter* targetCtx) const
{
    DateInstance* copy = new DateInstance(targetCtx->builtinDatePrototype());
    copy->setInternalValue(internalValue());
    return copy;
}

bool DateInstance::getTime(tm &t, int &offset) const
{
    double milli = internalValue()->getNumber();
    if (isNaN(milli))
        return false;

    millisecondsToTM(milli, false, &t);
    offset = gmtoffset(t);
    return true;
}

bool DateInstance::getUTCTime(tm &t) const
{
    double milli = internalValue()->getNumber();
    if (isNaN(milli))
        return false;

    millisecondsToTM(milli, true, &t);
    return true;
}

bool DateInstance::getTime(double &milli, int &offset) const
{
    milli = internalValue()->getNumber();
    if (isNaN(milli))
        return false;

    tm t;
    millisecondsToTM(milli, false, &t);
    offset = gmtoffset(t);
    return true;
}

bool DateInstance::getUTCTime(double &milli) const
{
    milli = internalValue()->getNumber();
    if (isNaN(milli))
        return false;

    return true;
}

static inline bool isTime_tSigned()
{
    time_t minusOne = (time_t)(-1);
    return minusOne < 0;
}

static void millisecondsToTM(double milli, bool utc, tm *t)
{
  // check whether time value is outside time_t's usual range
  // make the necessary transformations if necessary
  static bool time_tIsSigned = isTime_tSigned();
#if PLATFORM(WIN_OS)
  static double time_tMin = 0; //on windows localtime/gmtime returns NULL for pre 1970 dates
#else
  static double time_tMin = (time_tIsSigned ? - (double)(1ULL << (8 * sizeof(time_t) - 1)) : 0);
#endif
  static double time_tMax = (time_tIsSigned ? (1ULL << (8 * sizeof(time_t) - 1)) - 1 : 2 * (double)(1ULL << (8 * sizeof(time_t) - 1)) - 1);
  int realYearOffset = 0;
  double milliOffset = 0.0;
  double secs = floor(milli / msPerSecond);

  if (secs < time_tMin || secs > time_tMax) {
    // ### ugly and probably not very precise
    int realYear = yearFromTime(milli);
    int base = daysInYear(realYear) == 365 ? 2001 : 2000;
    milliOffset = timeFromYear(base) - timeFromYear(realYear);
    milli += milliOffset;
    realYearOffset = realYear - base;
  }

  time_t tv = (time_t) floor(milli / msPerSecond);

  *t = *(utc ? gmtime(&tv) : localtime(&tv));
  // We had an out of range year. Restore the year (plus/minus offset
  // found by calculating tm_year) and fix the week day calculation.
  if (realYearOffset != 0) {
    t->tm_year += realYearOffset;
    milli -= milliOffset;
    // Do our own weekday calculation. Use time zone offset to handle local time.
    double m = milli;
    if (!utc)
      m += gmtoffset(*t) * msPerSecond;
    t->tm_wday = weekDay(m);
  }
}

static bool isNaNorInf(double value)
{
    return isNaN(value) || isInf(value);
}

// ------------------------------ DatePrototype -----------------------------

const ClassInfo DatePrototype::info = {"Date", &DateInstance::info, &dateTable, 0};

/* Source for date_object.lut.h
   We use a negative ID to denote the "UTC" variant.
@begin dateTable 61
  toString		DateProtoFunc::ToString		DontEnum|Function	0
  toUTCString		-DateProtoFunc::ToUTCString		DontEnum|Function	0
  toDateString		DateProtoFunc::ToDateString		DontEnum|Function	0
  toTimeString		DateProtoFunc::ToTimeString		DontEnum|Function	0
  toISOString       DateProtoFunc::ToISOString      DontEnum|Function   0
  toJSON            DateProtoFunc::ToJSON           DontEnum|Function   1
  toLocaleString	DateProtoFunc::ToLocaleString	DontEnum|Function	0
  toLocaleDateString	DateProtoFunc::ToLocaleDateString	DontEnum|Function	0
  toLocaleTimeString	DateProtoFunc::ToLocaleTimeString	DontEnum|Function	0
  valueOf		DateProtoFunc::ValueOf		DontEnum|Function	0
  getTime		DateProtoFunc::GetTime		DontEnum|Function	0
  getFullYear		DateProtoFunc::GetFullYear		DontEnum|Function	0
  getUTCFullYear	-DateProtoFunc::GetFullYear		DontEnum|Function	0
  toGMTString		-DateProtoFunc::ToGMTString		DontEnum|Function	0
  getMonth		DateProtoFunc::GetMonth		DontEnum|Function	0
  getUTCMonth		-DateProtoFunc::GetMonth		DontEnum|Function	0
  getDate		DateProtoFunc::GetDate		DontEnum|Function	0
  getUTCDate		-DateProtoFunc::GetDate		DontEnum|Function	0
  getDay		DateProtoFunc::GetDay		DontEnum|Function	0
  getUTCDay		-DateProtoFunc::GetDay		DontEnum|Function	0
  getHours		DateProtoFunc::GetHours		DontEnum|Function	0
  getUTCHours		-DateProtoFunc::GetHours		DontEnum|Function	0
  getMinutes		DateProtoFunc::GetMinutes		DontEnum|Function	0
  getUTCMinutes		-DateProtoFunc::GetMinutes		DontEnum|Function	0
  getSeconds		DateProtoFunc::GetSeconds		DontEnum|Function	0
  getUTCSeconds		-DateProtoFunc::GetSeconds		DontEnum|Function	0
  getMilliseconds	DateProtoFunc::GetMilliSeconds	DontEnum|Function	0
  getUTCMilliseconds	-DateProtoFunc::GetMilliSeconds	DontEnum|Function	0
  getTimezoneOffset	DateProtoFunc::GetTimezoneOffset	DontEnum|Function	0
  setTime		DateProtoFunc::SetTime		DontEnum|Function	1
  setMilliseconds	DateProtoFunc::SetMilliSeconds	DontEnum|Function	1
  setUTCMilliseconds	-DateProtoFunc::SetMilliSeconds	DontEnum|Function	1
  setSeconds		DateProtoFunc::SetSeconds		DontEnum|Function	2
  setUTCSeconds		-DateProtoFunc::SetSeconds		DontEnum|Function	2
  setMinutes		DateProtoFunc::SetMinutes		DontEnum|Function	3
  setUTCMinutes		-DateProtoFunc::SetMinutes		DontEnum|Function	3
  setHours		DateProtoFunc::SetHours		DontEnum|Function	4
  setUTCHours		-DateProtoFunc::SetHours		DontEnum|Function	4
  setDate		DateProtoFunc::SetDate		DontEnum|Function	1
  setUTCDate		-DateProtoFunc::SetDate		DontEnum|Function	1
  setMonth		DateProtoFunc::SetMonth		DontEnum|Function	2
  setUTCMonth		-DateProtoFunc::SetMonth		DontEnum|Function	2
  setFullYear		DateProtoFunc::SetFullYear		DontEnum|Function	3
  setUTCFullYear	-DateProtoFunc::SetFullYear		DontEnum|Function	3
  setYear		DateProtoFunc::SetYear		DontEnum|Function	1
  getYear		DateProtoFunc::GetYear		DontEnum|Function	0
@end
*/
// ECMA 15.9.4

DatePrototype::DatePrototype(ExecState *, ObjectPrototype *objectProto)
  : DateInstance(objectProto)
{
    setInternalValue(jsNaN());
    // The constructor will be added later, after DateObjectImp has been built.
}

bool DatePrototype::getOwnPropertySlot(ExecState *exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticFunctionSlot<DateProtoFunc, JSObject>(exec, &dateTable, this, propertyName, slot);
}

// ------------------------------ DateProtoFunc -----------------------------

DateProtoFunc::DateProtoFunc(ExecState *exec, int i, int len, const Identifier& name)
  : InternalFunctionImp(static_cast<FunctionPrototype*>(exec->lexicalInterpreter()->builtinFunctionPrototype()), name)
  , id(abs(i))
  , utc(i < 0)
  // We use a negative ID to denote the "UTC" variant.
{
    putDirect(exec->propertyNames().length, len, DontDelete|ReadOnly|DontEnum);
}

JSValue *DateProtoFunc::callAsFunction(ExecState *exec, JSObject *thisObj, const List &args)
{
  if (id == ToJSON) {
    JSValue* tv = thisObj->toPrimitive(exec, NumberType);
    if (tv->isNumber()) {
      double ms = tv->toNumber(exec);
      if (isNaN(ms))
        return jsNull();
    }

    JSValue *toISO = thisObj->get(exec, exec->propertyNames().toISOString);
    if (!toISO->implementsCall())
      return throwError(exec, TypeError, "toISOString is not callable");
    JSObject* toISOobj = toISO->toObject(exec);
    if (!toISOobj)
      return throwError(exec, TypeError, "toISOString is not callable");
    return toISOobj->call(exec, thisObj, List::empty());
  }

  if (!thisObj->inherits(&DateInstance::info))
    return throwError(exec, TypeError);

  DateInstance* thisDateObj = static_cast<DateInstance*>(thisObj);

  JSValue *result = 0;
#if !PLATFORM(MAC)
  const int bufsize=100;
  char timebuffer[bufsize];
  CString oldlocale = setlocale(LC_TIME, 0);
  if (!oldlocale.size())
    oldlocale = setlocale(LC_ALL, 0);
  // FIXME: Where's the code to set the locale back to oldlocale?
#endif
  JSValue *v = thisDateObj->internalValue();
  double milli = v->toNumber(exec);
  if (isNaN(milli)) {
    switch (id) {
      case ToString:
      case ToDateString:
      case ToTimeString:
      case ToGMTString:
      case ToUTCString:
      case ToLocaleString:
      case ToLocaleDateString:
      case ToLocaleTimeString:
        return jsString("Invalid Date", 12);
      case ValueOf:
      case GetTime:
      case GetYear:
      case GetFullYear:
      case GetMonth:
      case GetDate:
      case GetDay:
      case GetHours:
      case GetMinutes:
      case GetSeconds:
      case GetMilliSeconds:
      case GetTimezoneOffset:
      case SetMilliSeconds:
      case SetSeconds:
      case SetMinutes:
      case SetHours:
      case SetDate:
      case SetMonth:
      case SetFullYear:
        return jsNaN();
      case ToISOString:
        return throwError(exec, RangeError, "Invalid Date");
    }
  }

  if (id == SetTime) {
    double milli = roundValue(exec, args[0]);
    result = jsNumber(timeClip(milli));
    thisDateObj->setInternalValue(result);
    return result;
  }

  double secs = floor(milli / msPerSecond);
  double ms = milli - secs * msPerSecond;

  tm t;
  millisecondsToTM(milli, utc, &t);

  switch (id) {
  case ToString:
    return jsString(formatDate(t).append(' ').append(formatTime(t, utc)));

  case ToDateString:
    return jsString(formatDate(t));

  case ToTimeString:
    return jsString(formatTime(t, utc));

  case ToGMTString:
  case ToUTCString:
    return jsString(formatDateUTCVariant(t).append(' ').append(formatTime(t, utc)));
  case ToISOString:
    return jsString(formatDateISOVariant(t, utc, milli).append('T').append(formatTimeISOVariant(t, utc, milli, ms)).append('Z'));

#if PLATFORM(MAC)
  case ToLocaleString:
    return jsString(formatLocaleDate(exec, secs, true, true, args));

  case ToLocaleDateString:
    return jsString(formatLocaleDate(exec, secs, true, false, args));

  case ToLocaleTimeString:
    return jsString(formatLocaleDate(exec, secs, false, true, args));

#else
  case ToLocaleString:
    return jsString(timebuffer, strftime(timebuffer, bufsize, "%c", &t));

  case ToLocaleDateString:
    return jsString(timebuffer, strftime(timebuffer, bufsize, "%x", &t));

  case ToLocaleTimeString:
    return jsString(timebuffer, strftime(timebuffer, bufsize, "%X", &t));

#endif
  case ValueOf:
  case GetTime:
    return jsNumber(milli);
  case GetYear:
    // IE returns the full year even in getYear.
    if (exec->dynamicInterpreter()->compatMode() == Interpreter::IECompat)
      return jsNumber(1900 + t.tm_year);
    return jsNumber(t.tm_year);
  case GetFullYear:
    return jsNumber(1900 + t.tm_year);
  case GetMonth:
    return jsNumber(t.tm_mon);
  case GetDate:
    return jsNumber(t.tm_mday);
  case GetDay:
    return jsNumber(t.tm_wday);
  case GetHours:
    return jsNumber(t.tm_hour);
  case GetMinutes:
    return jsNumber(t.tm_min);
  case GetSeconds:
    return jsNumber(t.tm_sec);
  case GetMilliSeconds:
    return jsNumber(ms);
  case GetTimezoneOffset:
    return jsNumber(-gmtoffset(t) / 60);

  case SetMilliSeconds:
  case SetSeconds:
  case SetMinutes:
  case SetHours:
    ms = args.size() > 0 ? setTimeFields(exec, args, id, ms, &t) : NaN;
    break;

  case SetDate:
  case SetMonth:
  case SetFullYear:
    ms = args.size() > 0 ? setDateFields(exec, args, id, ms, &t) : NaN;
    break;

  case SetYear: {
      int32_t year = args[0]->toInt32(exec);
      t.tm_year = (year > 99 || year < 0) ? year - 1900 : year;
      break;
    }
  }

  if (id == SetYear || id == SetMilliSeconds || id == SetSeconds ||
      id == SetMinutes || id == SetHours || id == SetDate ||
      id == SetMonth || id == SetFullYear ) {
    result = jsNumber(isnan(ms) ? ms : timeClip(makeTime(&t, ms, utc)));
    thisDateObj->setInternalValue(result);
  }

  return result;
}

// ------------------------------ DateObjectImp --------------------------------

DateObjectImp::DateObjectImp(ExecState *exec,
                             FunctionPrototype *funcProto,
                             DatePrototype *dateProto)
  : InternalFunctionImp(funcProto)
{
  // ECMA 15.9.4.1 Date.prototype
  static const Identifier* parsePropertyName = new Identifier("parse");
  static const Identifier* UTCPropertyName = new Identifier("UTC");
  static const Identifier* nowPropertyName = new Identifier("now");

  putDirect(exec->propertyNames().prototype, dateProto, DontEnum|DontDelete|ReadOnly);
  putDirectFunction(new DateObjectFuncImp(exec, funcProto, DateObjectFuncImp::Parse, 1, *parsePropertyName), DontEnum);
  putDirectFunction(new DateObjectFuncImp(exec, funcProto, DateObjectFuncImp::UTC, 7, *UTCPropertyName), DontEnum);
  putDirectFunction(new DateObjectFuncImp(exec, funcProto, DateObjectFuncImp::Now, 0, *nowPropertyName), DontEnum);

  // no. of arguments for constructor
  putDirect(exec->propertyNames().length, 7, ReadOnly|DontDelete|DontEnum);
}

bool DateObjectImp::implementsConstruct() const
{
    return true;
}

static double getCurrentUTCTime()
{
#if PLATFORM(WIN_OS)
#if COMPILER(BORLAND)
    struct timeb timebuffer;
    ftime(&timebuffer);
#else
    struct _timeb timebuffer;
    _ftime(&timebuffer);
#endif
    double utc = timebuffer.time * msPerSecond + timebuffer.millitm;
#else
    struct timeval tv;
    gettimeofday(&tv, 0);
    double utc = floor(tv.tv_sec * msPerSecond + tv.tv_usec / 1000);
#endif
    return utc;
}

static double makeTimeFromList(ExecState *exec, const List &args, bool utc)
{
    const int numArgs = args.size();
    if (isNaNorInf(args[0]->toNumber(exec))
        || isNaNorInf(args[1]->toNumber(exec))
        || (numArgs >= 3 && isNaNorInf(args[2]->toNumber(exec)))
        || (numArgs >= 4 && isNaNorInf(args[3]->toNumber(exec)))
        || (numArgs >= 5 && isNaNorInf(args[4]->toNumber(exec)))
        || (numArgs >= 6 && isNaNorInf(args[5]->toNumber(exec)))
        || (numArgs >= 7 && isNaNorInf(args[6]->toNumber(exec)))) {
      return NaN;
    }

    tm t;
    memset(&t, 0, sizeof(t));
    int year = args[0]->toInt32(exec);
    t.tm_year = (year >= 0 && year <= 99) ? year : year - 1900;
    t.tm_mon = args[1]->toInt32(exec);
    t.tm_mday = (numArgs >= 3) ? args[2]->toInt32(exec) : 1;
    t.tm_hour = (numArgs >= 4) ? args[3]->toInt32(exec) : 0;
    t.tm_min = (numArgs >= 5) ? args[4]->toInt32(exec) : 0;
    t.tm_sec = (numArgs >= 6) ? args[5]->toInt32(exec) : 0;
    if (!utc)
        t.tm_isdst = -1;
    double ms = (numArgs >= 7) ? roundValue(exec, args[6]) : 0;
    return makeTime(&t, ms, true);
}

// ECMA 15.9.3
JSObject *DateObjectImp::construct(ExecState *exec, const List &args)
{
  int numArgs = args.size();
  double value;

  if (numArgs == 0) { // new Date() ECMA 15.9.3.3
    value = getCurrentUTCTime();
  } else if (numArgs == 1) {
    JSValue* arg0 = args[0];
    if (arg0->isObject(&DateInstance::info))
      value = static_cast<DateInstance*>(arg0)->internalValue()->toNumber(exec);
    else {
      JSValue* primitive = arg0->toPrimitive(exec);
      if (primitive->isString())
        value = parseDate(primitive->getString());
      else
        value = primitive->toNumber(exec);
    }
  } else {
    value = makeTimeFromList(exec, args, false);
  }

  DateInstance *ret = new DateInstance(exec->lexicalInterpreter()->builtinDatePrototype());
  ret->setInternalValue(jsNumber(timeClip(value)));
  return ret;
}

// ECMA 15.9.2
JSValue *DateObjectImp::callAsFunction(ExecState * /*exec*/, JSObject * /*thisObj*/, const List &/*args*/)
{
    time_t t = time(0);
    tm ts = *localtime(&t);
    return jsString(formatDate(ts).append(' ').append(formatTime(ts, false)));
}

// ------------------------------ DateObjectFuncImp ----------------------------

DateObjectFuncImp::DateObjectFuncImp(ExecState* exec, FunctionPrototype* funcProto, int i, int len, const Identifier& name)
    : InternalFunctionImp(funcProto, name), id(i)
{
    putDirect(exec->propertyNames().length, len, DontDelete|ReadOnly|DontEnum);
}

// ECMA 15.9.4.2 - 3
JSValue *DateObjectFuncImp::callAsFunction(ExecState* exec, JSObject*, const List& args)
{
  if (id == Parse) {
    return jsNumber(parseDate(args[0]->toString(exec)));
  } else if (id == Now) {
    return jsNumber(getCurrentUTCTime());
  } else { // UTC
    return jsNumber(makeTimeFromList(exec, args, true));
  }
}

// -----------------------------------------------------------------------------

// Code originally from krfcdate.cpp, but we don't want to use kdecore, and we want double range.

static inline double ymdhmsToSeconds(long year, int mon, int day, int hour, int minute, double second)
{
    // in which case is the floor() needed? breaks day value of
    // "new Date('Thu Nov 5 2065 18:15:30 GMT+0500')"
#if 0
    double days = (day - 32075)
        + floor(1461 * (year + 4800.0 + (mon - 14) / 12) / 4)
        + 367 * (mon - 2 - (mon - 14) / 12 * 12) / 12
        - floor(3 * ((year + 4900.0 + (mon - 14) / 12) / 100) / 4)
        - 2440588;
#else
    double days = (day - 32075)
            + 1461 * (year + 4800 + (mon - 14) / 12) / 4
            + 367 * (mon - 2 - (mon - 14) / 12 * 12) / 12
            - 3 * ((year + 4900 + (mon - 14) / 12) / 100) / 4
            - 2440588;
#endif
    return ((days * hoursPerDay + hour) * minutesPerHour + minute) * secondsPerMinute + second;
}

// We follow the recommendation of RFC 2822 to consider all
// obsolete time zones not listed here equivalent to "-0000".
static const struct KnownZone {
#if !PLATFORM(WIN_OS)
    const
#endif
        char tzName[4];
    int tzOffset;
} known_zones[] = {
    { "UT", 0 },
    { "GMT", 0 },
    { "EST", -300 },
    { "EDT", -240 },
    { "CST", -360 },
    { "CDT", -300 },
    { "MST", -420 },
    { "MDT", -360 },
    { "PST", -480 },
    { "PDT", -420 }
};

#if PLATFORM(WIN_OS)
void FileTimeToUnixTime(LPFILETIME pft, double* pt)
{
    ULARGE_INTEGER ull;
    ull.LowPart = pft->dwLowDateTime;
    ull.HighPart = pft->dwHighDateTime;
    *pt = (double)(ull.QuadPart / 10000000ULL) - 11644473600ULL;
}

void SystemTimeToUnixTime(LPSYSTEMTIME pst, double* pt)
{
    FILETIME ft;
    SystemTimeToFileTime(pst, &ft);
    FileTimeToUnixTime(&ft, pt);
}
#endif

static double makeTime(tm *t, double ms, bool utc)
{
    int utcOffset;
    if (utc) {
        time_t zero = 0;
#if PLATFORM(WIN_OS)
        // FIXME: not thread safe
        (void)localtime(&zero);
#if COMPILER(BORLAND) || COMPILER(CYGWIN)
        utcOffset = - _timezone;
#else
        utcOffset = - timezone;
#endif
        t->tm_isdst = 0;
#elif PLATFORM(DARWIN)
        utcOffset = 0;
        t->tm_isdst = 0;
#else
        tm t3;
        localtime_r(&zero, &t3);
        utcOffset = gmtoffset(t3);
        t->tm_isdst = t3.tm_isdst;
#endif
    } else {
        utcOffset = 0;
        t->tm_isdst = -1;
    }

#if !PLATFORM(WIN_OS)
    double yearOffset = 0.0;
    if (t->tm_year < (1971 - 1900) || t->tm_year > (2037 - 1900)) {
        // we'll fool mktime() into believing that this year is within
        // its normal, portable range (1970-2038) by setting tm_year to
        // 2000 or 2001 and adding the difference in milliseconds later.
        // choice between offset will depend on whether the year is a
        // leap year or not.
        int y = t->tm_year + 1900;
        int baseYear = daysInYear(y) == 365 ? 2001 : 2000;
        double baseTime = timeFromYear(baseYear);
        yearOffset = timeFromYear(y) - baseTime;
        t->tm_year = baseYear - 1900;
    }

    // Determine whether DST is in effect. mktime() can't do this for us because
    // it doesn't know about ms and yearOffset.
    // NOTE: Casting values of large magnitude to time_t (long) will
    // produce incorrect results, but there's no other option when calling localtime_r().
    if (!utc) {
        time_t tval = mktime(t) + (time_t)((ms + yearOffset) / 1000);
        tm t3 = *localtime(&tval);
        t->tm_isdst = t3.tm_isdst;
    }

    return (mktime(t) + utcOffset) * msPerSecond + ms + yearOffset;
#else
    SYSTEMTIME st, dt;
    double tval;

    st.wYear = 1900 + t->tm_year;
    st.wMonth = t->tm_mon + 1;
    st.wDayOfWeek = t->tm_wday;
    st.wDay = t->tm_mday;
    st.wHour = t->tm_hour;
    st.wMinute = t->tm_min;
    st.wSecond = t->tm_sec;
    st.wMilliseconds = 0;

    TzSpecificLocalTimeToSystemTime(0, &st, &dt);        
    SystemTimeToUnixTime(&dt, &tval);

    return (tval + utcOffset) * msPerSecond + ms;
#endif
}

inline static bool isSpaceLike(char c)
{
    return isASCIISpace(c) || c == ',' || c == ':' || c == '-';
}

static const char* skipSpacesAndComments(const char* s)
{
    int nesting = 0;
    char ch;
    while ((ch = *s)) {
        // interpret - before a number as a sign rather than a comment char
        if (ch == '-' && isASCIIDigit(*(s+1)))
            break;
        if (!isSpaceLike(ch)) {
            if (ch == '(')
                nesting++;
            else if (ch == ')' && nesting > 0)
                nesting--;
            else if (nesting == 0)
                break;
        }
        s++;
    }
    return s;
}

// returns 0-11 (Jan-Dec); -1 on failure
static int findMonth(const char *monthStr)
{
    assert(monthStr);
    char needle[4];
    for (int i = 0; i < 3; ++i) {
        if (!*monthStr)
            return -1;
        needle[i] = toASCIILower(*monthStr++);
    }
    needle[3] = '\0';
    const char *haystack = "janfebmaraprmayjunjulaugsepoctnovdec";
    const char *str = strstr(haystack, needle);
    if (str) {
        int position = str - haystack;
        if (position % 3 == 0)
            return position / 3;
    }
    return -1;
}

static bool isTwoDigits (const char* str)
{
    return isASCIIDigit(str[0]) && isASCIIDigit(str[1]);
}

static int twoDigit (const char* str)
{
    return (str[0] - '0') * 10 + str[1] - '0';
}

static double parseDate(const UString &date)
{
    // This parses a date in the form:
    //     Tuesday, 09-Nov-99 23:12:40 GMT
    // or
    //     Sat, 01-Jan-2000 08:00:00 GMT
    // or
    //     Sat, 01 Jan 2000 08:00:00 GMT
    // or
    //     01 Jan 99 22:00 +0100    (exceptions in rfc822/rfc2822)
    // ### non RFC formats, added for Javascript:
    //     [Wednesday] January 09 1999 23:12:40 GMT
    //     [Wednesday] January 09 23:12:40 GMT 1999
    //
    // We ignore the weekday.

    CString dateCString = date.UTF8String();
    const char *dateString = dateCString.c_str();
    if(!dateString)
      return NaN;

    // Skip leading space
    dateString = skipSpacesAndComments(dateString);

    // ISO 8601: "YYYY-MM-DD('T'|'t')hh:mm:ss[.S+]['Z']"
    // e.g. "2006-06-15T23:12:10.207830Z"
    if (isTwoDigits(dateString) &&
        isTwoDigits(dateString + 2) &&
        dateString[4] == '-' &&
        isTwoDigits(dateString + 5) &&
        dateString[7] == '-' &&
        isTwoDigits(dateString + 8))
    {
        int year  = twoDigit(dateString) * 100 + twoDigit(dateString + 2);
        int month = twoDigit(dateString + 5) - 1;
        int day   = twoDigit(dateString + 8);
        if (month > 11 || day < 1 || day > 31)
            return NaN;
        int hour = 0, minute = 0;
        double second = 0;
        dateString += 10;
        if ((dateString[0] | 0x20) == 't' &&
            isTwoDigits(dateString + 1) &&
            dateString[3] == ':' &&
            isTwoDigits(dateString + 4))
        {
            hour   = twoDigit(dateString + 1);
            minute = twoDigit(dateString + 4);
            if (hour > 23 || minute > 59)
                return NaN;
            dateString += 6;
            if (dateString[0] == ':' &&
                isTwoDigits(dateString + 1))
            {
                second = twoDigit(dateString + 1);
                if (second > 59)
                    return NaN;
                dateString += 3;
                if (dateString[0] == '.' &&
                    isASCIIDigit(dateString[1]))
                {
                    dateString++;
                    double div = 10;
                    do {
                        second += (dateString[0] - '0') / div;
                        div *= 10;
                    } while (isASCIIDigit(*++dateString));
                }
            }
        }

        if (dateString[0] == 'Z')
        {
            tm t;
            memset(&t, 0, sizeof(tm));
            int secs = int(second);
            t.tm_sec   = secs;
            t.tm_min   = minute;
            t.tm_hour  = hour;
            t.tm_mday  = day;
            t.tm_mon   = month;
            t.tm_year  = year - 1900;
        //  t.tm_isdst = -1;

            // Use our makeTime() rather than mktime() as the latter can't handle the full year range.
            return makeTime(&t, (second - secs) * 1000, true);
        }

        int offset = 0;
        return (ymdhmsToSeconds(year, month + 1, day, hour, minute, second) - (offset * 60.0)) * msPerSecond;
    }

    long month = -1;
    const char *wordStart = dateString;
    // Check contents of first words if not number
    while (*dateString && !isASCIIDigit(*dateString)) {
        if (isASCIISpace(*dateString) || *dateString == '(') {
            if (dateString - wordStart >= 3)
                month = findMonth(wordStart);
            dateString = skipSpacesAndComments(dateString);
            wordStart = dateString;
        } else
           dateString++;
    }

    // Missing delimiter between month and day (like "January29")?
    if (month == -1 && wordStart != dateString)
        month = findMonth(wordStart);

    dateString = skipSpacesAndComments(dateString);

    if (!*dateString)
        return NaN;

    // ' 09-Nov-99 23:12:40 GMT'
    char *newPosStr;
    errno = 0;
    long day = strtol(dateString, &newPosStr, 10);
    dateString = newPosStr;

    if (errno || day < 0 || !*dateString)
        return NaN;

    long year = 0;
    if (day > 31) {
        // ### where is the boundary and what happens below?
        if (*dateString != '/')
            return NaN;
        // looks like a YYYY/MM/DD date
        if (!*++dateString)
            return NaN;
        year = day;
        month = strtol(dateString, &newPosStr, 10) - 1;
        if (errno)
            return NaN;
        dateString = newPosStr;
        if (*dateString++ != '/' || !*dateString)
            return NaN;
        day = strtol(dateString, &newPosStr, 10);
        if (errno)
            return NaN;
        dateString = newPosStr;
    } else if (*dateString == '/' && month == -1) {
        dateString++;
        // This looks like a MM/DD/YYYY date, not an RFC date.
        month = day - 1; // 0-based
        day = strtol(dateString, &newPosStr, 10);
        if (errno)
            return NaN;
        dateString = newPosStr;
        if (*dateString == '/')
            dateString++;
        if (!*dateString)
            return NaN;
     } else {
        if (*dateString == '-')
            dateString++;

        dateString = skipSpacesAndComments(dateString);

        if (*dateString == ',')
            dateString++;

        if (month == -1) { // not found yet
            month = findMonth(dateString);
            if (month == -1)
                return NaN;

            while (*dateString && (*dateString != '-') && !isASCIISpace(*dateString))
                dateString++;

            if (!*dateString)
                return NaN;

            // '-99 23:12:40 GMT'
            if (*dateString != '-' && *dateString != '/' && !isASCIISpace(*dateString))
                return NaN;
            dateString++;
        }

        if (month < 0 || month > 11)
            return NaN;
    }

    // '99 23:12:40 GMT'
    if (year <= 0 && *dateString) {
        year = strtol(dateString, &newPosStr, 10);
        if (errno)
            return NaN;
    }

    // Don't fail if the time is missing.
    long hour = 0;
    long minute = 0;
    long second = 0;
    if (!*newPosStr)
        dateString = newPosStr;
    else {
        // ' 23:12:40 GMT'
        if (*newPosStr == ':') {
            // There was no year; the number was the hour.
            year = -1;
        } else if (isSpaceLike(*newPosStr)) {
            // in the normal case (we parsed the year), advance to the next number
            dateString = skipSpacesAndComments(newPosStr + 1);
        } else {
            return NaN;
        }

        hour = strtol(dateString, &newPosStr, 10);
        // Do not check for errno here since we want to continue
        // even if errno was set because we are still looking
        // for the timezone!

        // Read a number? If not, this might be a timezone name.
        if (newPosStr != dateString) {
            dateString = newPosStr;

            if (hour < 0 || hour > 23)
                return NaN;

            if (!*dateString)
                return NaN;

            // ':12:40 GMT'
            if (*dateString++ != ':')
                return NaN;

            minute = strtol(dateString, &newPosStr, 10);
            if (errno)
                return NaN;
            dateString = newPosStr;

            if (minute < 0 || minute > 59)
                return NaN;

            // ':40 GMT'
            if (*dateString && *dateString != ':' && !isASCIISpace(*dateString))
                return NaN;

            // seconds are optional in rfc822 + rfc2822
            if (*dateString ==':') {
                dateString++;

                second = strtol(dateString, &newPosStr, 10);
                if (errno)
                    return NaN;
                dateString = newPosStr;

                if (second < 0 || second > 59)
                    return NaN;

                // disallow trailing colon seconds
                if (*dateString == ':')
                    return NaN;
            }

            dateString = skipSpacesAndComments(dateString);

            if (strncasecmp(dateString, "AM", 2) == 0) {
                if (hour > 12)
                    return NaN;
                if (hour == 12)
                    hour = 0;
                dateString = skipSpacesAndComments(dateString + 2);
            } else if (strncasecmp(dateString, "PM", 2) == 0) {
                if (hour > 12)
                    return NaN;
                if (hour != 12)
                    hour += 12;
                dateString = skipSpacesAndComments(dateString + 2);
            }
        }
    }

    bool haveTZ = false;
    int offset = 0;

    // Don't fail if the time zone is missing.
    // Some websites omit the time zone (4275206).
    if (*dateString) {
        if (strncasecmp(dateString, "GMT", 3) == 0 ||
            strncasecmp(dateString, "UTC", 3) == 0) {
            dateString += 3;
            haveTZ = true;
        }

        if (*dateString == '+' || *dateString == '-') {
            long o = strtol(dateString, &newPosStr, 10);
            if (errno)
                return NaN;
            dateString = newPosStr;

            if (o < -9959 || o > 9959)
                return NaN;

            int sgn = (o < 0) ? -1 : 1;
            o = abs(o);
            if (*dateString != ':') {
                offset = ((o / 100) * 60 + (o % 100)) * sgn;
            } else { // GMT+05:00
                dateString++;
                long o2 = strtol(dateString, &newPosStr, 10);
                if (errno)
                    return NaN;
                dateString = newPosStr;
                offset = (o * 60 + o2) * sgn;
            }
            haveTZ = true;
        } else {
            for (int i = 0; i < int(sizeof(known_zones) / sizeof(KnownZone)); i++) {
                if (0 == strncasecmp(dateString, known_zones[i].tzName, strlen(known_zones[i].tzName))) {
                    offset = known_zones[i].tzOffset;
                    dateString += strlen(known_zones[i].tzName);
                    haveTZ = true;
                    break;
                }
            }
        }
    }

    dateString = skipSpacesAndComments(dateString);

    if (*dateString && year == -1) {
        year = strtol(dateString, &newPosStr, 10);
        if (errno)
            return NaN;
        dateString = newPosStr;
    }

    dateString = skipSpacesAndComments(dateString);

    // Trailing garbage
    if (*dateString)
        return NaN;

    // Y2K: Handle 2 digit years.
    if (year >= 0 && year < 100) {
        if (year < 50)
            year += 2000;
        else
            year += 1900;
    }

    // fall back to local timezone
    if (!haveTZ) {
        tm t;
        memset(&t, 0, sizeof(tm));
        t.tm_mday = day;
        t.tm_mon = month;
        t.tm_year = year - 1900;
        t.tm_isdst = -1;
        t.tm_sec = second;
        t.tm_min = minute;
        t.tm_hour = hour;

        // Use our makeTime() rather than mktime() as the latter can't handle the full year range.
        return makeTime(&t, 0, false);
    }

    return (ymdhmsToSeconds(year, month + 1, day, hour, minute, second) - (offset * 60.0)) * msPerSecond;
}

double timeClip(double t)
{
    if (isnan(t) || isInf(t))
        return NaN;
    double at = fabs(t);
    if (at > 8.64E15)
        return NaN;
    return copysign(floor(at), t);
}

}   // namespace KJS
