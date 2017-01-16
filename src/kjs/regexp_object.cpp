// krazy:excludeall=doublequote_chars (UStrings aren't QStrings)
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2003 Apple Computer, Inc.
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

#include "regexp_object.h"

#include "regexp_object.lut.h"

#include <stdio.h>
#include "value.h"
#include "object.h"
#include "types.h"
#include "nodes.h"
#include "interpreter.h"
#include "operations.h"
#include "internal.h"
#include "regexp.h"
#include "error_object.h"
#include "lookup.h"

using namespace KJS;

// ------------------------------ RegExpPrototype ---------------------------

// ECMA 15.10.5

const ClassInfo RegExpPrototype::info = {"RegExp", nullptr, nullptr, nullptr};

RegExpPrototype::RegExpPrototype(ExecState *exec,
                                 ObjectPrototype *objProto,
                                 FunctionPrototype *funcProto)
    : JSObject(objProto)
{
    static const Identifier *execPropertyName = new Identifier("exec");
    static const Identifier *testPropertyName = new Identifier("test");
    static const Identifier *compilePropertyName = new Identifier("compile");

    putDirectFunction(new RegExpProtoFunc(exec, funcProto, RegExpProtoFunc::Exec, 0, *execPropertyName), DontEnum);
    putDirectFunction(new RegExpProtoFunc(exec, funcProto, RegExpProtoFunc::Test, 0, *testPropertyName), DontEnum);
    putDirectFunction(new RegExpProtoFunc(exec, funcProto, RegExpProtoFunc::ToString, 0,  exec->propertyNames().toString), DontEnum);
    putDirectFunction(new RegExpProtoFunc(exec, funcProto, RegExpProtoFunc::Compile,  1, *compilePropertyName), DontEnum);
}

// ------------------------------ RegExpProtoFunc ---------------------------

RegExpProtoFunc::RegExpProtoFunc(ExecState *exec, FunctionPrototype *funcProto, int i, int len, const Identifier &name)
    : InternalFunctionImp(funcProto, name), id(i)
{
    putDirect(exec->propertyNames().length, len, DontDelete | ReadOnly | DontEnum);
}

JSValue *RegExpProtoFunc::callAsFunction(ExecState *exec, JSObject *thisObj, const List &args)
{
    if (!thisObj->inherits(&RegExpImp::info)) {
        if (thisObj->inherits(&RegExpPrototype::info)) {
            switch (id) {
            case ToString: return jsString("//");
            }
        }

        return throwError(exec, TypeError);
    }

    switch (id) {
    case Test:      // 15.10.6.2
    case Exec: {
        RegExp *regExp = static_cast<RegExpImp *>(thisObj)->regExp();
        RegExpObjectImp *regExpObj = static_cast<RegExpObjectImp *>(exec->lexicalInterpreter()->builtinRegExp());

        UString input;
        if (args.isEmpty()) {
            input = regExpObj->get(exec, exec->propertyNames().input)->toString(exec);
        } else {
            input = args[0]->toString(exec);
        }

        double lastIndex = thisObj->get(exec, exec->propertyNames().lastIndex)->toInteger(exec);

        bool globalFlag = thisObj->get(exec, exec->propertyNames().global)->toBoolean(exec);
        if (!globalFlag) {
            lastIndex = 0;
        }
        if (lastIndex < 0 || lastIndex > input.size()) {
            thisObj->put(exec, exec->propertyNames().lastIndex, jsNumber(0), DontDelete | DontEnum);
            return jsNull();
        }

        int foundIndex;

        RegExpStringContext ctx(input);
        UString match = regExpObj->performMatch(regExp, exec, ctx, input, static_cast<int>(lastIndex), &foundIndex);

        if (exec->hadException()) {
            return jsUndefined();
        }

        bool didMatch = !match.isNull();

        if (globalFlag) {
            if (didMatch) {
                thisObj->put(exec, exec->propertyNames().lastIndex, jsNumber(foundIndex + match.size()), DontDelete | DontEnum);
            } else {
                thisObj->put(exec, exec->propertyNames().lastIndex, jsNumber(0), DontDelete | DontEnum);
            }
        }

        // Test
        if (id == Test) {
            return jsBoolean(didMatch);
        }

        // Exec
        if (didMatch) {
            return regExpObj->arrayOfMatches(exec, match);
        } else {
            return jsNull();
        }
    }
    break;
    case ToString: {
        UString result = "/" + thisObj->get(exec, exec->propertyNames().source)->toString(exec) + "/";
        if (thisObj->get(exec, exec->propertyNames().global)->toBoolean(exec)) {
            result += "g";
        }
        if (thisObj->get(exec, exec->propertyNames().ignoreCase)->toBoolean(exec)) {
            result += "i";
        }
        if (thisObj->get(exec, exec->propertyNames().multiline)->toBoolean(exec)) {
            result += "m";
        }
        return jsString(result);
    }
    case Compile: { // JS1.2 legacy, but still in use in the wild somewhat
        RegExpImp *instance = static_cast<RegExpImp *>(thisObj);
        RegExp *newEngine   = RegExpObjectImp::makeEngine(exec, args[0]->toString(exec), args[1]);
        if (!newEngine) {
            return exec->exception();
        }
        instance->setRegExp(exec, newEngine);
        return instance;
    }
    }

    return jsUndefined();
}

// ------------------------------ RegExpImp ------------------------------------

const ClassInfo RegExpImp::info = {"RegExp", nullptr, nullptr, nullptr};

RegExpImp::RegExpImp(RegExpPrototype *regexpProto)
    : JSObject(regexpProto), reg(nullptr)
{
}

RegExpImp::~RegExpImp()
{
    delete reg;
}

void RegExpImp::setRegExp(ExecState *exec, RegExp *r)
{
    delete reg;
    reg = r;

    putDirect(exec->propertyNames().global, jsBoolean(r->flags() & RegExp::Global), DontDelete | ReadOnly | DontEnum);
    putDirect(exec->propertyNames().ignoreCase, jsBoolean(r->flags() & RegExp::IgnoreCase), DontDelete | ReadOnly | DontEnum);
    putDirect(exec->propertyNames().multiline,  jsBoolean(r->flags() & RegExp::Multiline), DontDelete | ReadOnly | DontEnum);

    putDirect(exec->propertyNames().source, jsString(r->pattern()), DontDelete | ReadOnly | DontEnum);
    putDirect(exec->propertyNames().lastIndex, jsNumber(0), DontDelete | DontEnum);
}

JSObject *RegExpImp::valueClone(Interpreter *targetCtx) const
{
    RegExpImp *copy = new RegExpImp(static_cast<RegExpPrototype *>(targetCtx->builtinRegExpPrototype()));
    copy->setRegExp(targetCtx->globalExec(), new RegExp(reg->pattern(), reg->flags()));
    return copy;
}

// ------------------------------ RegExpObjectImp ------------------------------

const ClassInfo RegExpObjectImp::info = {"Function", &InternalFunctionImp::info, &RegExpTable, nullptr};

/* Source for regexp_object.lut.h
@begin RegExpTable 20
  input           RegExpObjectImp::Input          None
  $_              RegExpObjectImp::Input          DontEnum
  multiline       RegExpObjectImp::Multiline      None
  $*              RegExpObjectImp::Multiline      DontEnum
  lastMatch       RegExpObjectImp::LastMatch      DontDelete|ReadOnly
  $&              RegExpObjectImp::LastMatch      DontDelete|ReadOnly|DontEnum
  lastParen       RegExpObjectImp::LastParen      DontDelete|ReadOnly
  $+              RegExpObjectImp::LastParen      DontDelete|ReadOnly|DontEnum
  leftContext     RegExpObjectImp::LeftContext    DontDelete|ReadOnly
  $`              RegExpObjectImp::LeftContext    DontDelete|ReadOnly|DontEnum
  rightContext    RegExpObjectImp::RightContext   DontDelete|ReadOnly
  $'              RegExpObjectImp::RightContext   DontDelete|ReadOnly|DontEnum
  $1              RegExpObjectImp::Dollar1        DontDelete|ReadOnly
  $2              RegExpObjectImp::Dollar2        DontDelete|ReadOnly
  $3              RegExpObjectImp::Dollar3        DontDelete|ReadOnly
  $4              RegExpObjectImp::Dollar4        DontDelete|ReadOnly
  $5              RegExpObjectImp::Dollar5        DontDelete|ReadOnly
  $6              RegExpObjectImp::Dollar6        DontDelete|ReadOnly
  $7              RegExpObjectImp::Dollar7        DontDelete|ReadOnly
  $8              RegExpObjectImp::Dollar8        DontDelete|ReadOnly
  $9              RegExpObjectImp::Dollar9        DontDelete|ReadOnly
@end
*/

struct KJS::RegExpObjectImpPrivate {
    // Global search cache / settings
    RegExpObjectImpPrivate() : lastInput(""), lastNumSubPatterns(0), multiline(false) { }
    UString lastInput;
    OwnArrayPtr<int> lastOvector;
    unsigned lastNumSubPatterns : 31;
    bool multiline              : 1;
};

RegExpObjectImp::RegExpObjectImp(ExecState *exec,
                                 FunctionPrototype *funcProto,
                                 RegExpPrototype *regProto)

    : InternalFunctionImp(funcProto),
      d(new RegExpObjectImpPrivate)
{
    // ECMA 15.10.5.1 RegExp.prototype
    putDirect(exec->propertyNames().prototype, regProto, DontEnum | DontDelete | ReadOnly);

    // no. of arguments for constructor
    putDirect(exec->propertyNames().length, jsNumber(2), ReadOnly | DontDelete | DontEnum);
}

void RegExpObjectImp::throwRegExpError(ExecState *exec)
{
    throwError(exec, RangeError, "Resource exhaustion trying to perform regexp match.");
}

/*
  To facilitate result caching, exec(), test(), match(), search(), and replace() dipatch regular
  expression matching through the performMatch function. We use cached results to calculate,
  e.g., RegExp.lastMatch and RegExp.leftParen.
*/
UString RegExpObjectImp::performMatch(RegExp *r, ExecState *exec, const RegExpStringContext &c,
                                      const UString &s,
                                      int startOffset, int *endOffset, int **ovector)
{
    int tmpOffset;
    int *tmpOvector;
    bool error = false;
    UString match = r->match(c, s, &error, startOffset, &tmpOffset, &tmpOvector);
    if (error) {
        if (endOffset) {
            *endOffset = -1;
        }
        throwRegExpError(exec);
        return match;
    }

    if (endOffset) {
        *endOffset = tmpOffset;
    }
    if (ovector) {
        *ovector = tmpOvector;
    }

    if (!match.isNull()) {
        ASSERT(tmpOvector);

        d->lastInput = s;
        d->lastOvector.set(tmpOvector);
        d->lastNumSubPatterns = r->subPatterns();
    }

    return match;
}

JSObject *RegExpObjectImp::arrayOfMatches(ExecState *exec, const UString &result) const
{
    List list;
    // The returned array contains 'result' as first item, followed by the list of matches
    list.append(jsString(result));
    if (d->lastOvector)
        for (int i = 1; i < d->lastNumSubPatterns + 1; ++i) {
            int start = d->lastOvector[2 * i];
            if (start == -1) {
                list.append(jsUndefined());
            } else {
                UString substring = d->lastInput.substr(start, d->lastOvector[2 * i + 1] - start);
                list.append(jsString(substring));
            }
        }
    JSObject *arr = exec->lexicalInterpreter()->builtinArray()->construct(exec, list);
    arr->put(exec, exec->propertyNames().index, jsNumber(d->lastOvector[0]));
    arr->put(exec, exec->propertyNames().input, jsString(d->lastInput));
    return arr;
}

JSValue *RegExpObjectImp::getBackref(int i) const
{
    if (d->lastOvector && i < int(d->lastNumSubPatterns + 1)) {
        UString substring = d->lastInput.substr(d->lastOvector[2 * i], d->lastOvector[2 * i + 1] - d->lastOvector[2 * i]);
        return jsString(substring);
    }

    return jsString("");
}

JSValue *RegExpObjectImp::getLastMatch() const
{
    if (d->lastOvector) {
        UString substring = d->lastInput.substr(d->lastOvector[0], d->lastOvector[1] - d->lastOvector[0]);
        return jsString(substring);
    }

    return jsString("");
}

JSValue *RegExpObjectImp::getLastParen() const
{
    int i = d->lastNumSubPatterns;
    if (i > 0) {
        ASSERT(d->lastOvector);
        UString substring = d->lastInput.substr(d->lastOvector[2 * i], d->lastOvector[2 * i + 1] - d->lastOvector[2 * i]);
        return jsString(substring);
    }

    return jsString("");
}

JSValue *RegExpObjectImp::getLeftContext() const
{
    if (d->lastOvector) {
        UString substring = d->lastInput.substr(0, d->lastOvector[0]);
        return jsString(substring);
    }

    return jsString("");
}

JSValue *RegExpObjectImp::getRightContext() const
{
    if (d->lastOvector) {
        UString s = d->lastInput;
        UString substring = s.substr(d->lastOvector[1], s.size() - d->lastOvector[1]);
        return jsString(substring);
    }

    return jsString("");
}

bool RegExpObjectImp::getOwnPropertySlot(ExecState *exec, const Identifier &propertyName, PropertySlot &slot)
{
    return getStaticValueSlot<RegExpObjectImp, InternalFunctionImp>(exec, &RegExpTable, this, propertyName, slot);
}

JSValue *RegExpObjectImp::getValueProperty(ExecState *, int token) const
{
    switch (token) {
    case Dollar1:
        return getBackref(1);
    case Dollar2:
        return getBackref(2);
    case Dollar3:
        return getBackref(3);
    case Dollar4:
        return getBackref(4);
    case Dollar5:
        return getBackref(5);
    case Dollar6:
        return getBackref(6);
    case Dollar7:
        return getBackref(7);
    case Dollar8:
        return getBackref(8);
    case Dollar9:
        return getBackref(9);
    case Input:
        return jsString(d->lastInput);
    case Multiline:
        return jsBoolean(d->multiline);
    case LastMatch:
        return getLastMatch();
    case LastParen:
        return getLastParen();
    case LeftContext:
        return getLeftContext();
    case RightContext:
        return getRightContext();
    default:
        ASSERT(0);
    }

    return jsString("");
}

void RegExpObjectImp::put(ExecState *exec, const Identifier &propertyName, JSValue *value, int attr)
{
    lookupPut<RegExpObjectImp, InternalFunctionImp>(exec, propertyName, value, attr, &RegExpTable, this);
}

void RegExpObjectImp::putValueProperty(ExecState *exec, int token, JSValue *value, int /*attr*/)
{
    switch (token) {
    case Input:
        d->lastInput = value->toString(exec);
        break;
    case Multiline:
        d->multiline = value->toBoolean(exec);
        break;
    default:
        ASSERT(0);
    }
}

bool RegExpObjectImp::implementsConstruct() const
{
    return true;
}

RegExp *RegExpObjectImp::makeEngine(ExecState *exec, const UString &p, JSValue *flagsInput)
{
    int reflags = RegExp::None;

    if (!flagsInput->isUndefined()) {
        const UString flags = flagsInput->toString(exec);

        // Check flags
        for (int pos = 0; pos < flags.size(); ++pos) {
            switch (flags[pos].unicode()) {
            case 'g':
                if (reflags & RegExp::Global) {
                    throwError(exec, SyntaxError,
                               "Regular expression flag 'g' given twice", 1, -1, "<regexp>");
                    return nullptr;
                }
                reflags |= RegExp::Global;
                break;
            case 'i':
                if (reflags & RegExp::IgnoreCase) {
                    throwError(exec, SyntaxError,
                               "Regular expression flag 'i' given twice", 1, -1, "<regexp>");
                    return nullptr;
                }
                reflags |= RegExp::IgnoreCase;
                break;
            case 'm':
                if (reflags & RegExp::Multiline) {
                    throwError(exec, SyntaxError,
                               "Regular expression flag 'm' given twice", 1, -1, "<regexp>");
                    return nullptr;
                }
                reflags |= RegExp::Multiline;
                break;
            default: {
                throwError(exec, SyntaxError,
                           "Invalid regular expression flags", 1, -1, "<regexp>");
                return nullptr;
            }
            }
        }
    }

    RegExp *re = new RegExp(p, reflags);
    if (!re->isValid()) {
        throwError(exec, SyntaxError,
                   "Invalid regular expression", 1, -1, "<regexp>");
        delete re;
        return nullptr;
    }
    return re;
}

// ECMA 15.10.4
JSObject *RegExpObjectImp::construct(ExecState *exec, const List &args)
{
    JSObject *o = args[0]->getObject();
    if (o && o->inherits(&RegExpImp::info)) {
        if (!args[1]->isUndefined()) {
            return throwError(exec, TypeError);
        }
        return o;
    }

    UString p = args[0]->isUndefined() ? UString("") : args[0]->toString(exec);

    RegExp *re = makeEngine(exec, p, args[1]);
    if (!re) {
        return exec->exception()->toObject(exec);
    }

    RegExpPrototype *proto = static_cast<RegExpPrototype *>(exec->lexicalInterpreter()->builtinRegExpPrototype());
    RegExpImp *dat = new RegExpImp(proto);

    dat->setRegExp(exec, re);

    return dat;
}

// ECMA 15.10.3
JSValue *RegExpObjectImp::callAsFunction(ExecState *exec, JSObject * /*thisObj*/, const List &args)
{
    // The RegExp argument case is handled by construct()

    return construct(exec, args);
}
