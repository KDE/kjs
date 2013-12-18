// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
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

#include "string_object.h"
#include "string_object.lut.h"

#include "JSWrapperObject.h"
#include "error_object.h"
#include "operations.h"
#include "PropertyNameArray.h"
#include "regexp_object.h"
#include "commonunicode.h"
#include <wtf/unicode/libc/UnicodeLibC.h>

#if PLATFORM(WIN_OS)
#include <windows.h>
#endif

using namespace WTF;

namespace KJS {

// ------------------------------ StringInstance ----------------------------

const ClassInfo StringInstance::info = {"String", 0, 0, 0};

StringInstance::StringInstance(JSObject *proto)
  : JSWrapperObject(proto), m_conversionsCustomized(false)
{
  setInternalValue(jsString(""));
}

StringInstance::StringInstance(JSObject *proto, StringImp* string)
  : JSWrapperObject(proto), m_conversionsCustomized(false)
{
  setInternalValue(string);
}

StringInstance::StringInstance(JSObject *proto, const UString &string)
  : JSWrapperObject(proto), m_conversionsCustomized(false)
{
  setInternalValue(jsString(string));
}

JSValue *StringInstance::lengthGetter(ExecState*, JSObject*, const Identifier&, const PropertySlot &slot)
{
    return jsNumber(static_cast<StringInstance*>(slot.slotBase())->internalValue()->value().size());
}

JSValue *StringInstance::indexGetter(ExecState*, JSObject*, unsigned, const PropertySlot &slot)
{
    const UChar c = static_cast<StringInstance*>(slot.slotBase())->internalValue()->value()[slot.index()];
    return jsString(UString(&c, 1));
}

bool StringInstance::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    if (propertyName == exec->propertyNames().length) {
        slot.setCustom(this, lengthGetter);
        return true;
    }

    bool isStrictUInt32;
    unsigned i = propertyName.toStrictUInt32(&isStrictUInt32);
    unsigned length = internalValue()->value().size();
    if (isStrictUInt32 && i < length) {
        slot.setCustomIndex(this, i, indexGetter);
        return true;
    }
    
    return JSObject::getOwnPropertySlot(exec, propertyName, slot);
}
    
bool StringInstance::getOwnPropertySlot(ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    unsigned length = internalValue()->value().size();
    if (propertyName < length) {
        slot.setCustomIndex(this, propertyName, indexGetter);
        return true;
    }
    
    return JSObject::getOwnPropertySlot(exec, Identifier::from(propertyName), slot);
}

bool StringInstance::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& desc)
{
    if (propertyName == exec->propertyNames().length) {
        desc.setPropertyDescriptorValues(exec, jsNumber(internalValue()->value().size()), ReadOnly|DontDelete|DontEnum);
        return true;
    }

    return KJS::JSObject::getOwnPropertyDescriptor(exec, propertyName, desc);
}

void StringInstance::put(ExecState *exec, const Identifier &propertyName, JSValue *value, int attr)
{
  if (propertyName == exec->propertyNames().length)
    return;

  if (propertyName == exec->propertyNames().valueOf || propertyName == exec->propertyNames().toString)
    m_conversionsCustomized = true;

  JSObject::put(exec, propertyName, value, attr);
}

bool StringInstance::deleteProperty(ExecState *exec, const Identifier &propertyName)
{
  if (propertyName == exec->propertyNames().length)
    return false;
  return JSObject::deleteProperty(exec, propertyName);
}

void StringInstance::getOwnPropertyNames(ExecState* exec, PropertyNameArray& propertyNames, PropertyMap::PropertyMode mode)
{
  int size = internalValue()->getString().size();
  for (int i = 0; i < size; i++)
    propertyNames.add(Identifier(UString::from(i)));

  if (mode == PropertyMap::IncludeDontEnumProperties)
    propertyNames.add(exec->propertyNames().length);

  return JSObject::getOwnPropertyNames(exec, propertyNames, mode);
}

UString StringInstance::toString(ExecState *exec) const
{
  if (prototype() == originalProto() && !conversionsCustomized() &&
      !static_cast<StringPrototype*>(prototype())->conversionsCustomized())
    return internalValue()->value();
  else
    return JSObject::toString(exec);
}

JSObject* StringInstance::valueClone(Interpreter* targetCtx) const
{
    return new StringInstance(targetCtx->builtinStringPrototype(), internalValue());
}

// ------------------------------ StringPrototype ---------------------------
const ClassInfo StringPrototype::info = {"String", &StringInstance::info, &stringTable, 0};
/* Source for string_object.lut.h
@begin stringTable 26
  toString              StringProtoFunc::ToString       DontEnum|Function       0
  valueOf               StringProtoFunc::ValueOf        DontEnum|Function       0
  charAt                StringProtoFunc::CharAt         DontEnum|Function       1
  charCodeAt            StringProtoFunc::CharCodeAt     DontEnum|Function       1
  concat                StringProtoFunc::Concat         DontEnum|Function       1
  indexOf               StringProtoFunc::IndexOf        DontEnum|Function       1
  lastIndexOf           StringProtoFunc::LastIndexOf    DontEnum|Function       1
  match                 StringProtoFunc::Match          DontEnum|Function       1
  replace               StringProtoFunc::Replace        DontEnum|Function       2
  search                StringProtoFunc::Search         DontEnum|Function       1
  slice                 StringProtoFunc::Slice          DontEnum|Function       2
  split                 StringProtoFunc::Split          DontEnum|Function       2
  substr                StringProtoFunc::Substr         DontEnum|Function       2
  substring             StringProtoFunc::Substring      DontEnum|Function       2
  toLowerCase           StringProtoFunc::ToLowerCase    DontEnum|Function       0
  toUpperCase           StringProtoFunc::ToUpperCase    DontEnum|Function       0
  toLocaleLowerCase     StringProtoFunc::ToLocaleLowerCase DontEnum|Function    0
  toLocaleUpperCase     StringProtoFunc::ToLocaleUpperCase DontEnum|Function    0
  trim                  StringProtoFunc::Trim           DontEnum|Function       0
  localeCompare         StringProtoFunc::LocaleCompare  DontEnum|Function       1
#
# Under here: html extension, should only exist if KJS_PURE_ECMA is not defined
# I guess we need to generate two hashtables in the .lut.h file, and use #ifdef
# to select the right one... TODO. #####
  big                   StringProtoFunc::Big            DontEnum|Function       0
  small                 StringProtoFunc::Small          DontEnum|Function       0
  blink                 StringProtoFunc::Blink          DontEnum|Function       0
  bold                  StringProtoFunc::Bold           DontEnum|Function       0
  fixed                 StringProtoFunc::Fixed          DontEnum|Function       0
  italics               StringProtoFunc::Italics        DontEnum|Function       0
  strike                StringProtoFunc::Strike         DontEnum|Function       0
  sub                   StringProtoFunc::Sub            DontEnum|Function       0
  sup                   StringProtoFunc::Sup            DontEnum|Function       0
  fontcolor             StringProtoFunc::Fontcolor      DontEnum|Function       1
  fontsize              StringProtoFunc::Fontsize       DontEnum|Function       1
  anchor                StringProtoFunc::Anchor         DontEnum|Function       1
  link                  StringProtoFunc::Link           DontEnum|Function       1
  trimLeft              StringProtoFunc::TrimLeft       DontEnum|Function       0
  trimRight             StringProtoFunc::TrimRight      DontEnum|Function       0
@end
*/
// ECMA 15.5.4
StringPrototype::StringPrototype(ExecState* exec, ObjectPrototype* objProto)
  : StringInstance(objProto)
{
  // The constructor will be added later, after StringObjectImp has been built
  putDirect(exec->propertyNames().length, jsNumber(0), DontDelete | ReadOnly | DontEnum);
}

bool StringPrototype::getOwnPropertySlot(ExecState *exec, const Identifier& propertyName, PropertySlot &slot)
{
  return getStaticFunctionSlot<StringProtoFunc, StringInstance>(exec, &stringTable, this, propertyName, slot);
}

// ------------------------------ StringProtoFunc ---------------------------

StringProtoFunc::StringProtoFunc(ExecState* exec, int i, int len, const Identifier& name)
  : InternalFunctionImp(static_cast<FunctionPrototype*>(exec->lexicalInterpreter()->builtinFunctionPrototype()), name)
  , id(i)
{
  putDirect(exec->propertyNames().length, len, DontDelete | ReadOnly | DontEnum);
}

static inline void expandSourceRanges(UString::Range * & array, int& count, int& capacity)
{
  int newCapacity;
  if (capacity == 0) {
    newCapacity = 16;
  } else {
    newCapacity = capacity * 2;
  }

  UString::Range *newArray = new UString::Range[newCapacity];
  for (int i = 0; i < count; i++) {
    newArray[i] = array[i];
  }

  delete [] array;

  capacity = newCapacity;
  array = newArray;
}

static void pushSourceRange(UString::Range * & array, int& count, int& capacity, UString::Range range)
{
  if (count + 1 > capacity)
    expandSourceRanges(array, count, capacity);

  array[count] = range;
  count++;
}

static inline void expandReplacements(UString * & array, int& count, int& capacity)
{
  int newCapacity;
  if (capacity == 0) {
    newCapacity = 16;
  } else {
    newCapacity = capacity * 2;
  }

  UString *newArray = new UString[newCapacity];
  for (int i = 0; i < count; i++) {
    newArray[i] = array[i];
  }

  delete [] array;

  capacity = newCapacity;
  array = newArray;
}

static void pushReplacement(UString * & array, int& count, int& capacity, UString replacement)
{
  if (count + 1 > capacity)
    expandReplacements(array, count, capacity);

  array[count] = replacement;
  count++;
}

static inline UString substituteBackreferences(const UString &replacement, const UString &source, int *ovector, RegExp *reg)
{
  UString substitutedReplacement = replacement;

  int i = -1;
  while ((i = substitutedReplacement.find(UString("$"), i + 1)) != -1) {
    if (i+1 == substitutedReplacement.size())
        break;

    unsigned short ref = substitutedReplacement[i+1].unicode();
    int backrefStart = 0;
    int backrefLength = 0;
    int advance = 0;

    if (ref == '$') {  // "$$" -> "$"
        substitutedReplacement = substitutedReplacement.substr(0, i + 1) + substitutedReplacement.substr(i + 2);
        continue;
    } else if (ref == '&') {
        backrefStart = ovector[0];
        backrefLength = ovector[1] - backrefStart;
    } else if (ref == '`') {
        backrefStart = 0;
        backrefLength = ovector[0];
    } else if (ref == '\'') {
        backrefStart = ovector[1];
        backrefLength = source.size() - backrefStart;
    } else if (ref >= '0' && ref <= '9') {
        // 1- and 2-digit back references are allowed
        unsigned backrefIndex = ref - '0';
        if (backrefIndex > (unsigned)reg->subPatterns())
            continue;
        if (substitutedReplacement.size() > i + 2) {
            ref = substitutedReplacement[i+2].unicode();
            if (ref >= '0' && ref <= '9') {
                backrefIndex = 10 * backrefIndex + ref - '0';
                if (backrefIndex > (unsigned)reg->subPatterns())
                    backrefIndex = backrefIndex / 10;   // Fall back to the 1-digit reference
                else
                    advance = 1;
            }
        }
        backrefStart = ovector[2 * backrefIndex];
        backrefLength = ovector[2 * backrefIndex + 1] - backrefStart;
    } else
        continue;

    substitutedReplacement = substitutedReplacement.substr(0, i) + source.substr(backrefStart, backrefLength) + substitutedReplacement.substr(i + 2 + advance);
    i += backrefLength - 1; // - 1 offsets 'i + 1'
  }

  return substitutedReplacement;
}

static inline int localeCompare(const UString& a, const UString& b)
{
#if PLATFORM(WIN_OS)
    int retval = CompareStringW(LOCALE_USER_DEFAULT, 0,
                                reinterpret_cast<LPCWSTR>(a.data()), a.size(),
                                reinterpret_cast<LPCWSTR>(b.data()), b.size());
    return !retval ? retval : retval - 2;
#elif PLATFORM(CF)
    CFStringRef sa = CFStringCreateWithCharactersNoCopy(kCFAllocatorDefault, reinterpret_cast<const UniChar*>(a.data()), a.size(), kCFAllocatorNull);
    CFStringRef sb = CFStringCreateWithCharactersNoCopy(kCFAllocatorDefault, reinterpret_cast<const UniChar*>(b.data()), b.size(), kCFAllocatorNull);

    int retval = CFStringCompare(sa, sb, kCFCompareLocalized);

    CFRelease(sa);
    CFRelease(sb);

    return retval;
#else
    // ### use as fallback only. implement locale aware version.
    // ### other browsers have more detailed return values than -1, 0 and 1
    return compare(a, b);
#endif
}

static JSValue *replace(ExecState *exec, const UString &source, JSValue *pattern, JSValue *replacement)
{
  JSObject *replacementFunction = 0;
  UString replacementString;

  if (replacement->isObject() && replacement->toObject(exec)->implementsCall())
    replacementFunction = replacement->toObject(exec);
  else
    replacementString = replacement->toString(exec);

  if (pattern->isObject() && static_cast<JSObject *>(pattern)->inherits(&RegExpImp::info)) {
    RegExp *reg = static_cast<RegExpImp *>(pattern)->regExp();
    bool global = reg->flags() & RegExp::Global;

    RegExpObjectImp* regExpObj = static_cast<RegExpObjectImp*>(exec->lexicalInterpreter()->builtinRegExp());

    int matchIndex = 0;
    int lastIndex = 0;
    int startPosition = 0;

    UString::Range *sourceRanges = 0;
    int sourceRangeCount = 0;
    int sourceRangeCapacity = 0;
    UString *replacements = 0;
    int replacementCount = 0;
    int replacementCapacity = 0;

    // This is either a loop (if global is set) or a one-way (if not).
    RegExpStringContext ctx(source);
    do {
      int *ovector;
      UString matchString = regExpObj->performMatch(reg, exec, ctx, source, startPosition, &matchIndex, &ovector);
      if (matchIndex == -1)
        break;
      int matchLen = matchString.size();

      pushSourceRange(sourceRanges, sourceRangeCount, sourceRangeCapacity, UString::Range(lastIndex, matchIndex - lastIndex));
      UString substitutedReplacement;
      if (replacementFunction) {
          int completeMatchStart = ovector[0];
          List args;

          args.append(jsString(matchString));

          for (unsigned i = 0; i < reg->subPatterns(); i++) {
              int matchStart = ovector[(i + 1) * 2];
              int matchLen = ovector[(i + 1) * 2 + 1] - matchStart;

              args.append(jsString(source.substr(matchStart, matchLen)));
          }

          args.append(jsNumber(completeMatchStart));
          args.append(jsString(source));

          substitutedReplacement = replacementFunction->call(exec, exec->dynamicInterpreter()->globalObject(),
                                                        args)->toString(exec);
      } else {
          substitutedReplacement = substituteBackreferences(replacementString, source, ovector, reg);
      }
      pushReplacement(replacements, replacementCount, replacementCapacity, substitutedReplacement);

      lastIndex = matchIndex + matchLen;
      startPosition = lastIndex;

      // special case of empty match
      if (matchLen == 0) {
        startPosition++;
        if (startPosition > source.size())
          break;
      }
    } while (global);


    if (lastIndex < source.size())
      pushSourceRange(sourceRanges, sourceRangeCount, sourceRangeCapacity, UString::Range(lastIndex, source.size() - lastIndex));

    UString result;
    if (sourceRanges)
        result = source.spliceSubstringsWithSeparators(sourceRanges, sourceRangeCount, replacements, replacementCount);

    delete [] sourceRanges;
    delete [] replacements;

    return jsString(result);
  }

  // First arg is a string
  UString patternString = pattern->toString(exec);
  int matchPos = source.find(patternString);
  int matchLen = patternString.size();
  // Do the replacement
  if (matchPos == -1)
    return jsString(source);

  if (replacementFunction) {
      List args;

      args.append(jsString(source.substr(matchPos, matchLen)));
      args.append(jsNumber(matchPos));
      args.append(jsString(source));

      replacementString = replacementFunction->call(exec, exec->dynamicInterpreter()->globalObject(),
                                                    args)->toString(exec);
  }

  return jsString(source.substr(0, matchPos) + replacementString + source.substr(matchPos + matchLen));
}

static UnicodeSupport::StringConversionFunction toUpperF = Unicode::toUpper;
static UnicodeSupport::StringConversionFunction toLowerF = Unicode::toLower;

void StringProtoFunc::setToLowerFunction(UnicodeSupport::StringConversionFunction f)
{
  toLowerF = f;
}

void StringProtoFunc::setToUpperFunction(UnicodeSupport::StringConversionFunction f)
{
  toUpperF = f;
}

// ECMA 15.5.4.2 - 15.5.4.20
JSValue *StringProtoFunc::callAsFunction(ExecState* exec, JSObject* thisObj, const List& args)
{
  JSValue* result = NULL;

  // toString and valueOf are no generic function.
  if (id == ToString || id == ValueOf) {
    if (!thisObj || !thisObj->inherits(&StringInstance::info))
      return throwError(exec, TypeError);

    return jsString(static_cast<StringInstance*>(thisObj)->internalValue()->toString(exec));
  }

  UString u, u2, u3;
  int pos, p0, i;
  double dpos;
  double d = 0.0;

  UString s = thisObj->toString(exec);

  int len = s.size();
  JSValue *a0 = args[0];
  JSValue *a1 = args[1];

  switch (id) {
  case ToString:
  case ValueOf:
    // handled above
    break;
  case CharAt:
    // Other browsers treat an omitted parameter as 0 rather than NaN.
    // That doesn't match the ECMA standard, but is needed for site compatibility.
    dpos = a0->isUndefined() ? 0 : a0->toInteger(exec);
    if (dpos >= 0 && dpos < len) // false for NaN
      u = s.substr(static_cast<int>(dpos), 1);
    else
      u = "";
    result = jsString(u);
    break;
  case CharCodeAt:
    // Other browsers treat an omitted parameter as 0 rather than NaN.
    // That doesn't match the ECMA standard, but is needed for site compatibility.
    dpos = a0->isUndefined() ? 0 : a0->toInteger(exec);
    if (dpos >= 0 && dpos < len) // false for NaN
      result = jsNumber(s[static_cast<int>(dpos)].unicode());
    else
      result = jsNaN();
    break;
  case Concat: {
    ListIterator it = args.begin();
    for ( ; it != args.end() ; ++it) {
        s += it->toString(exec);
    }
    result = jsString(s);
    break;
  }
  case IndexOf:
    u2 = a0->toString(exec);
    if (a1->isUndefined())
      dpos = 0;
    else {
      dpos = a1->toInteger(exec);
      if (dpos >= 0) { // false for NaN
        if (dpos > len)
          dpos = len;
      } else
        dpos = 0;
    }
    result = jsNumber(s.find(u2, static_cast<int>(dpos)));
    break;
  case LastIndexOf:
    u2 = a0->toString(exec);
    d = a1->toNumber(exec);
    if (a1->isUndefined() || KJS::isNaN(d))
      dpos = len;
    else {
      dpos = a1->toInteger(exec);
      if (dpos >= 0) { // false for NaN
        if (dpos > len)
          dpos = len;
      } else
        dpos = 0;
    }
    result = jsNumber(s.rfind(u2, static_cast<int>(dpos)));
    break;
  case Match:
  case Search: {
    u = s;
    RegExp *reg, *tmpReg = 0;
    RegExpImp *imp = 0;
    if (a0->isObject() && static_cast<JSObject *>(a0)->inherits(&RegExpImp::info)) {
      reg = static_cast<RegExpImp *>(a0)->regExp();
    } else {
      /*
       *  ECMA 15.5.4.12 String.prototype.search (regexp)
       *  If regexp is not an object whose [[Class]] property is "RegExp", it is
       *  replaced with the result of the expression new RegExp(regexp).
       */
      reg = tmpReg = new RegExp(a0->toString(exec), RegExp::None);
    }
    if (!reg->isValid()) {
      delete tmpReg;
      return throwError(exec, SyntaxError, "Invalid regular expression");
    }
    RegExpObjectImp* regExpObj = static_cast<RegExpObjectImp*>(exec->lexicalInterpreter()->builtinRegExp());

    RegExpStringContext ctx(u);
    UString mstr = regExpObj->performMatch(reg, exec, ctx, u, 0, &pos);

    if (id == Search) {
      result = jsNumber(pos);
    } else {
      // Exec
      if ((reg->flags() & RegExp::Global) == 0) {
        // case without 'g' flag is handled like RegExp.prototype.exec
        if (mstr.isNull()) {
          result = jsNull();
        } else {
          result = regExpObj->arrayOfMatches(exec,mstr);
        }
      } else {
        // return array of matches
        List list;
        int lastIndex = 0;
        while (pos >= 0) {
          if (mstr.isNull())
            list.append(jsUndefined());
          else
            list.append(jsString(mstr));
          lastIndex = pos;
          pos += mstr.isEmpty() ? 1 : mstr.size();
          mstr = regExpObj->performMatch(reg, exec, ctx, u, pos, &pos);
        }
        if (imp)
          imp->put(exec, "lastIndex", jsNumber(lastIndex), DontDelete|DontEnum);
        if (list.isEmpty()) {
          // if there are no matches at all, it's important to return
          // Null instead of an empty array, because this matches
          // other browsers and because Null is a false value.
          result = jsNull();
        } else {
          result = exec->lexicalInterpreter()->builtinArray()->construct(exec, list);
        }
      }
    }

    delete tmpReg;
    break;
  }
  case Replace:
    result = replace(exec, s, a0, a1);
    break;
  case Slice:
    {
      // The arg processing is very much like ArrayProtoFunc::Slice
      double start = a0->toInteger(exec);
      double end = a1->isUndefined() ? len : a1->toInteger(exec);
      double from = start < 0 ? len + start : start;
      double to = end < 0 ? len + end : end;
      if (to > from && to > 0 && from < len) {
        if (from < 0)
          from = 0;
        if (to > len)
          to = len;
        result = jsString(s.substr(static_cast<int>(from), static_cast<int>(to - from)));
      } else {
        result = jsString("");
      }
      break;
    }
    case Split: {
    JSObject *constructor = exec->lexicalInterpreter()->builtinArray();
    JSObject *res = static_cast<JSObject *>(constructor->construct(exec,List::empty()));
    result = res;
    u = s;
    i = p0 = 0;
    uint32_t limit = a1->isUndefined() ? 0xFFFFFFFFU : a1->toUInt32(exec);
    if (a0->isObject() && static_cast<JSObject *>(a0)->inherits(&RegExpImp::info)) {
      RegExp *reg = static_cast<RegExpImp *>(a0)->regExp();

      RegExpStringContext ctx(u);
      bool error = false;
      if (u.isEmpty() && !reg->match(ctx, u, &error, 0).isNull()) {

        // empty string matched by regexp -> empty array
        res->put(exec, exec->propertyNames().length, jsNumber(0));
        break;
      }
      pos = 0;
      while (!error && static_cast<uint32_t>(i) != limit && pos < u.size()) {
        // TODO: back references
        int mpos;
        int *ovector = 0L;
        UString mstr = reg->match(ctx, u, &error, pos, &mpos, &ovector);
        delete [] ovector; ovector = 0L;
        if (mpos < 0)
          break;
        pos = mpos + (mstr.isEmpty() ? 1 : mstr.size());
        if (mpos != p0 || !mstr.isEmpty()) {
          res->put(exec,i, jsString(u.substr(p0, mpos-p0)));
          p0 = mpos + mstr.size();
          i++;
        }
      }

      if (error)
        RegExpObjectImp::throwRegExpError(exec);

    } else {
      u2 = a0->toString(exec);
      if (u2.isEmpty()) {
        if (u.isEmpty()) {
          // empty separator matches empty string -> empty array
          put(exec, exec->propertyNames().length, jsNumber(0));
          break;
        } else {
          while (static_cast<uint32_t>(i) != limit && i < u.size()-1)
            res->put(exec, i++, jsString(u.substr(p0++, 1)));
        }
      } else {
        while (static_cast<uint32_t>(i) != limit && (pos = u.find(u2, p0)) >= 0) {
          res->put(exec, i, jsString(u.substr(p0, pos-p0)));
          p0 = pos + u2.size();
          i++;
        }
      }
    }
    // add remaining string, if any
    if (static_cast<uint32_t>(i) != limit)
      res->put(exec, i++, jsString(u.substr(p0)));
    res->put(exec, exec->propertyNames().length, jsNumber(i));
    }
    break;
  case Substr: { //B.2.3
    // Note: NaN is effectively handled as 0 here for both length
    // and start, hence toInteger does fine, and removes worries
    // about weird comparison results below.
    int len = s.size();
    double start  = a0->toInteger(exec);
    double length = a1->isUndefined() ? len : a1->toInteger(exec);

    if (start >= len)
      return jsString("");
    if (length < 0)
      return jsString("");
    if (start < 0) {
      start += s.size();
      if (start < 0)
       start = 0;
    }

    if (length > len - start)
      length = len - start;

    result = jsString(s.substr(static_cast<int>(start), static_cast<int>(length)));
    break;
  }
  case Substring: {
    double start = a0->toNumber(exec);
    double end = a1->toNumber(exec);
    if (isNaN(start))
      start = 0;
    if (isNaN(end))
      end = 0;
    if (start < 0)
      start = 0;
    if (end < 0)
      end = 0;
    if (start > len)
      start = len;
    if (end > len)
      end = len;
    if (a1->isUndefined())
      end = len;
    if (start > end) {
      double temp = end;
      end = start;
      start = temp;
    }
    result = jsString(s.substr((int)start, (int)end-(int)start));
    }
    break;
  case ToLowerCase:
  case ToLocaleLowerCase: { // FIXME: See http://www.unicode.org/Public/UNIDATA/SpecialCasing.txt for locale-sensitive mappings that aren't implemented.
    u = s;
    u.copyForWriting();
    uint16_t* dataPtr = reinterpret_cast<uint16_t*>(u.rep()->data());
    uint16_t* destIfNeeded;

    int len = toLowerF(dataPtr, u.size(), destIfNeeded);
    if (len >= 0)
        result = jsString(UString(reinterpret_cast<UChar*>(destIfNeeded ? destIfNeeded : dataPtr), len));
    else
        result = jsString(s);

    free(destIfNeeded);
    break;
  }
  case ToUpperCase:
  case ToLocaleUpperCase: { // FIXME: See http://www.unicode.org/Public/UNIDATA/SpecialCasing.txt for locale-sensitive mappings that aren't implemented.
    u = s;
    u.copyForWriting();
    uint16_t* dataPtr = reinterpret_cast<uint16_t*>(u.rep()->data());
    uint16_t* destIfNeeded;

    int len = toUpperF(dataPtr, u.size(), destIfNeeded);
    if (len >= 0)
        result = jsString(UString(reinterpret_cast<UChar *>(destIfNeeded ? destIfNeeded : dataPtr), len));
    else
        result = jsString(s);

    free(destIfNeeded);
    break;
  }
  case LocaleCompare:
    if (args.size() < 1)
      return jsNumber(0);
    return jsNumber(localeCompare(s, a0->toString(exec)));
  case Trim:
  case TrimRight:
  case TrimLeft: {
    const uint16_t* dataPtr = reinterpret_cast<uint16_t*>(s.rep()->data());

    const int size = s.size();
    int left = 0;
    if (id != TrimRight)
        while (left < size && CommonUnicode::isStrWhiteSpace(dataPtr[left]))
            left++;

    int right = size;
    if (id != TrimLeft)
        while (right > left && CommonUnicode::isStrWhiteSpace(dataPtr[right-1]))
            right--;

    result = jsString(s.substr(left, right-left));
    break;
  }
#ifndef KJS_PURE_ECMA
  case Big:
    result = jsString("<big>" + s + "</big>");
    break;
  case Small:
    result = jsString("<small>" + s + "</small>");
    break;
  case Blink:
    result = jsString("<blink>" + s + "</blink>");
    break;
  case Bold:
    result = jsString("<b>" + s + "</b>");
    break;
  case Fixed:
    result = jsString("<tt>" + s + "</tt>");
    break;
  case Italics:
    result = jsString("<i>" + s + "</i>");
    break;
  case Strike:
    result = jsString("<strike>" + s + "</strike>");
    break;
  case Sub:
    result = jsString("<sub>" + s + "</sub>");
    break;
  case Sup:
    result = jsString("<sup>" + s + "</sup>");
    break;
  case Fontcolor:
    result = jsString("<font color=\"" + a0->toString(exec) + "\">" + s + "</font>");
    break;
  case Fontsize:
    result = jsString("<font size=\"" + a0->toString(exec) + "\">" + s + "</font>");
    break;
  case Anchor:
    result = jsString("<a name=\"" + a0->toString(exec) + "\">" + s + "</a>");
    break;
  case Link:
    result = jsString("<a href=\"" + a0->toString(exec) + "\">" + s + "</a>");
    break;
#endif
  }

  return result;
}

// ------------------------------ StringObjectImp ------------------------------

StringObjectImp::StringObjectImp(ExecState* exec,
                                 FunctionPrototype* funcProto,
                                 StringPrototype* stringProto)
  : InternalFunctionImp(funcProto)
{
  // ECMA 15.5.3.1 String.prototype
  putDirect(exec->propertyNames().prototype, stringProto, DontEnum|DontDelete|ReadOnly);

  putDirectFunction(new StringObjectFuncImp(exec, funcProto, exec->propertyNames().fromCharCode), DontEnum);

  // no. of arguments for constructor
  putDirect(exec->propertyNames().length, jsNumber(1), ReadOnly|DontDelete|DontEnum);
}


bool StringObjectImp::implementsConstruct() const
{
  return true;
}

// ECMA 15.5.2
JSObject *StringObjectImp::construct(ExecState *exec, const List &args)
{
  JSObject *proto = exec->lexicalInterpreter()->builtinStringPrototype();
  if (args.size() == 0)
    return new StringInstance(proto);
  return new StringInstance(proto, args.begin()->toString(exec));
}

// ECMA 15.5.1
JSValue *StringObjectImp::callAsFunction(ExecState *exec, JSObject* /*thisObj*/, const List &args)
{
  if (args.isEmpty())
    return jsString("");
  else {
    JSValue *v = args[0];
    return jsString(v->toString(exec));
  }
}

// ------------------------------ StringObjectFuncImp --------------------------

// ECMA 15.5.3.2 fromCharCode()
StringObjectFuncImp::StringObjectFuncImp(ExecState* exec, FunctionPrototype* funcProto, const Identifier& name)
  : InternalFunctionImp(funcProto, name)
{
  putDirect(exec->propertyNames().length, jsNumber(1), DontDelete|ReadOnly|DontEnum);
}

JSValue *StringObjectFuncImp::callAsFunction(ExecState *exec, JSObject* /*thisObj*/, const List &args)
{
  UString s;
  if (args.size()) {
    UChar *buf = static_cast<UChar *>(fastMalloc(args.size() * sizeof(UChar)));
    UChar *p = buf;
    ListIterator it = args.begin();
    while (it != args.end()) {
      unsigned short u = it->toUInt16(exec);
      *p++ = UChar(u);
      it++;
    }
    s = UString(buf, args.size(), false);
  } else
    s = "";

  return jsString(s);
}

}
