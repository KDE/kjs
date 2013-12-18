// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003 Apple Computer, Inc.
 *  Copyright (C) 2008, 2009 Maksim Orlovich (maksim@kde.org)
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

#include "interpreter.h"

#include "SavedBuiltins.h"
#include "array_object.h"
#include "bool_object.h"
#include "collector.h"
#include "date_object.h"
#include "debugger.h"
#include "error_object.h"
#include "function_object.h"
#include "internal.h"
#include "math_object.h"
#include "nodes.h"
#include "number_object.h"
#include "object.h"
#include "object_object.h"
#include "operations.h"
#include "regexp_object.h"
#include "string_object.h"
#include "types.h"
#include "value.h"
#include "lexer.h"
#include "json_object.h"

#if USE(BINDINGS)
#include "runtime.h"
#endif

#if defined _WIN32 || defined _WIN64
#undef HAVE_SYS_TIME_H // no setitimer in kdewin32
#endif
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <assert.h>
#include <cstdlib>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

namespace KJS {

class TimeoutChecker {
public:
    void startTimeoutCheck(Interpreter*);
    void stopTimeoutCheck(Interpreter*);
    void pauseTimeoutCheck(Interpreter*);
    void resumeTimeoutCheck(Interpreter*);

private:
#if HAVE_SYS_TIME_H
    static Interpreter* s_executingInterpreter;
    static void alarmHandler(int);

    Interpreter* m_oldInterpreter;
    itimerval m_oldtv;
    itimerval m_pausetv;
    void (*m_oldAlarmHandler)(int);
#endif
};

#if HAVE_SYS_TIME_H
Interpreter* TimeoutChecker::s_executingInterpreter = 0;
#endif

void TimeoutChecker::startTimeoutCheck(Interpreter *interpreter)
{
    if (!interpreter->m_timeoutTime)
        return;

    interpreter->m_startTimeoutCheckCount++;

#if HAVE_SYS_TIME_H
    if (s_executingInterpreter == interpreter)
        return;

    // Block signals
    m_oldAlarmHandler = signal(SIGALRM, SIG_IGN);

    m_oldInterpreter = s_executingInterpreter;
    s_executingInterpreter = interpreter;

    itimerval tv = {
      { interpreter->m_timeoutTime / 1000, (interpreter->m_timeoutTime % 1000) * 1000 },
      { interpreter->m_timeoutTime / 1000, (interpreter->m_timeoutTime % 1000) * 1000 }
    };
    setitimer(ITIMER_REAL, &tv, &m_oldtv);

    // Unblock signals
    signal(SIGALRM, alarmHandler);
#endif
}

void TimeoutChecker::stopTimeoutCheck(Interpreter* interpreter)
{
    if (!interpreter->m_timeoutTime)
        return;

    ASSERT(interpreter->m_startTimeoutCheckCount > 0);

    interpreter->m_startTimeoutCheckCount--;

    if (interpreter->m_startTimeoutCheckCount != 0)
        return;

#if HAVE_SYS_TIME_H
    signal(SIGALRM, SIG_IGN);

    s_executingInterpreter = m_oldInterpreter;

    setitimer(ITIMER_REAL, &m_oldtv, 0L);
    signal(SIGALRM, m_oldAlarmHandler);
#endif
}

#if HAVE_SYS_TIME_H
void TimeoutChecker::alarmHandler(int)
{
    s_executingInterpreter->m_timedOut = true;
}
#endif

void TimeoutChecker::pauseTimeoutCheck(Interpreter* interpreter)
{
    if (interpreter->m_startTimeoutCheckCount == 0)
        return;

#if HAVE_SYS_TIME_H
    ASSERT(interpreter == s_executingInterpreter);

    void (*currentSignalHandler)(int);

    // Block signal
    currentSignalHandler = signal(SIGALRM, SIG_IGN);

    if (currentSignalHandler != alarmHandler) {
        signal(SIGALRM, currentSignalHandler);
        return;
    }

    setitimer(ITIMER_REAL, 0L, &m_pausetv);
#endif

    interpreter->m_pauseTimeoutCheckCount++;
}

void TimeoutChecker::resumeTimeoutCheck(Interpreter* interpreter)
{
    if (interpreter->m_startTimeoutCheckCount == 0)
        return;

#if HAVE_SYS_TIME_H
    ASSERT(interpreter == s_executingInterpreter);
#endif

    interpreter->m_pauseTimeoutCheckCount--;

    if (interpreter->m_pauseTimeoutCheckCount != 0)
        return;

#if HAVE_SYS_TIME_H
    void (*currentSignalHandler)(int);

    // Check so we have the right handler
    currentSignalHandler = signal(SIGALRM, SIG_IGN);

    if (currentSignalHandler != SIG_IGN) {
        signal(SIGALRM, currentSignalHandler);
        return;
    }

    setitimer(ITIMER_REAL, &m_pausetv, 0L);

    // Unblock signal
    currentSignalHandler = signal(SIGALRM, alarmHandler);
#endif
}

Interpreter* Interpreter::s_hook = 0;


Interpreter::Interpreter(JSGlobalObject* globalObject)
    : m_globalObject(globalObject),
      m_globalExec(this, globalObject),
      globPkg(0)
{
    init();
}

Interpreter::Interpreter()
    : m_globalObject(new JSGlobalObject()),
      m_globalExec(this, m_globalObject),
      globPkg(0)
{
    init();
}

void Interpreter::init()
{
    JSLock lock;

    initInternedStringsTable();

    m_refCount = 0;
    m_timeoutTime = 0;
    m_recursion = 0;
    m_debugger= 0;
    m_execState = 0;
    m_timedOut = false;
    m_timeoutChecker = 0;
    m_startTimeoutCheckCount = 0;
    m_pauseTimeoutCheckCount = 0;
    m_compatMode = NativeMode;

    const int initialStackSize = 8192;
    stackBase = (unsigned char*)std::malloc(initialStackSize);
    stackPtr  = stackBase;
    stackEnd  = stackBase + initialStackSize;

    m_numCachedActivations = 0;

    m_globalObject->setInterpreter(this);

    if (s_hook) {
        prev = s_hook;
        next = s_hook->next;
        s_hook->next->prev = this;
        s_hook->next = this;
    } else {
        // This is the first interpreter
        s_hook = next = prev = this;
    }

    initGlobalObject();
}

Interpreter::~Interpreter()
{
    JSLock lock;

    ASSERT (m_startTimeoutCheckCount == 0);
    ASSERT (m_pauseTimeoutCheckCount == 0);

    delete m_timeoutChecker;

    if (m_debugger)
        m_debugger->detach(this);

    std::free(stackBase);

    next->prev = prev;
    prev->next = next;
    s_hook = next;
    if (s_hook == this) {
        // This was the last interpreter
        s_hook = 0;
    }

    m_globalObject->setInterpreter(0);
}

unsigned char* Interpreter::extendStack(size_t needed)
{
    unsigned char* oldBase = stackBase; // needed for fixing up localStores

    size_t curSize = stackEnd - stackBase;
    size_t avail = stackEnd - stackPtr;
    size_t extra = needed - avail;

    if (extra < 8192)
        extra = 8192;
    size_t newSize = curSize + extra;

    //printf("Grow stack:%d -> %d\n", curSize, newSize);

    stackBase = (unsigned char*)std::malloc(newSize); // Not realloc since we need the old stuff
                                                      // ### seems optimizeable
    std::memcpy(stackBase, oldBase, curSize);
    stackPtr  = stackBase + (stackPtr - oldBase);
    stackEnd  = stackBase + newSize;

    // Now go through and fix up activations..
    ExecState* e = m_execState;
    while (e) {
        if (e->codeType() == FunctionCode) {
            ActivationImp* act = static_cast<ActivationImp*>(e->activationObject());
            if (act->localStorage) {
                act->localStorage = (LocalStorageEntry*)
                                      (stackBase + ((unsigned char*)act->localStorage - oldBase));
                e->updateLocalStorage(act->localStorage);
            }
        }

	e = e->savedExecState();
    }

    std::free(oldBase);

    return stackAlloc(needed);
}

void Interpreter::recycleActivation(ActivationImp* act)
{
    ASSERT(act->localStorage == 0); // Should not refer to anything by now
    if (m_numCachedActivations >= MaxCachedActivations)
        return;

    act->clearProperties();
    m_cachedActivations[m_numCachedActivations] = act;
    ++m_numCachedActivations;
}

JSGlobalObject* Interpreter::globalObject() const
{
  return m_globalObject;
}

void Interpreter::putNamedConstructor(const char* name, JSObject* value)
{
    assert(value->implementsCall());
    Identifier i(name);
    m_globalObject->put(&m_globalExec, i, value, DontEnum);
    static_cast<InternalFunctionImp*>(value)->setFunctionName(i);
}

void Interpreter::initGlobalObject()
{
    FunctionPrototype *funcProto = new FunctionPrototype(&m_globalExec);
    m_FunctionPrototype = funcProto;
    ObjectPrototype *objProto = new ObjectPrototype(&m_globalExec, funcProto);
    m_ObjectPrototype = objProto;
    funcProto->setPrototype(m_ObjectPrototype);

    ArrayPrototype *arrayProto = new ArrayPrototype(&m_globalExec, objProto);
    m_ArrayPrototype = arrayProto;
    StringPrototype *stringProto = new StringPrototype(&m_globalExec, objProto);
    m_StringPrototype = stringProto;
    BooleanPrototype *booleanProto = new BooleanPrototype(&m_globalExec, objProto, funcProto);
    m_BooleanPrototype = booleanProto;
    NumberPrototype *numberProto = new NumberPrototype(&m_globalExec, objProto, funcProto);
    m_NumberPrototype = numberProto;
    DatePrototype *dateProto = new DatePrototype(&m_globalExec, objProto);
    m_DatePrototype = dateProto;
    RegExpPrototype *regexpProto = new RegExpPrototype(&m_globalExec, objProto, funcProto);
    m_RegExpPrototype = regexpProto;
    ErrorPrototype *errorProto = new ErrorPrototype(&m_globalExec, objProto, funcProto);
    m_ErrorPrototype = errorProto;

    JSObject* o = m_globalObject;
    while (o->prototype()->isObject())
        o = static_cast<JSObject*>(o->prototype());
    o->setPrototype(m_ObjectPrototype);

    // Constructors (Object, Array, etc.)
    m_Object = new ObjectObjectImp(&m_globalExec, objProto, funcProto);
    m_Function = new FunctionObjectImp(&m_globalExec, funcProto);
    m_Array = new ArrayObjectImp(&m_globalExec, funcProto, arrayProto);
    m_String = new StringObjectImp(&m_globalExec, funcProto, stringProto);
    m_Boolean = new BooleanObjectImp(&m_globalExec, funcProto, booleanProto);
    m_Number = new NumberObjectImp(&m_globalExec, funcProto, numberProto);
    m_Date = new DateObjectImp(&m_globalExec, funcProto, dateProto);
    m_RegExp = new RegExpObjectImp(&m_globalExec, funcProto, regexpProto);
    m_Error = new ErrorObjectImp(&m_globalExec, funcProto, errorProto);

    // Error object prototypes
    m_EvalErrorPrototype = new NativeErrorPrototype(&m_globalExec, errorProto, EvalError, "EvalError", "EvalError");
    m_RangeErrorPrototype = new NativeErrorPrototype(&m_globalExec, errorProto, RangeError, "RangeError", "RangeError");
    m_ReferenceErrorPrototype = new NativeErrorPrototype(&m_globalExec, errorProto, ReferenceError, "ReferenceError", "ReferenceError");
    m_SyntaxErrorPrototype = new NativeErrorPrototype(&m_globalExec, errorProto, SyntaxError, "SyntaxError", "SyntaxError");
    m_TypeErrorPrototype = new NativeErrorPrototype(&m_globalExec, errorProto, TypeError, "TypeError", "TypeError");
    m_UriErrorPrototype = new NativeErrorPrototype(&m_globalExec, errorProto, URIError, "URIError", "URIError");

    // Error objects
    m_EvalError = new NativeErrorImp(&m_globalExec, funcProto, m_EvalErrorPrototype);
    m_RangeError = new NativeErrorImp(&m_globalExec, funcProto, m_RangeErrorPrototype);
    m_ReferenceError = new NativeErrorImp(&m_globalExec, funcProto, m_ReferenceErrorPrototype);
    m_SyntaxError = new NativeErrorImp(&m_globalExec, funcProto, m_SyntaxErrorPrototype);
    m_TypeError = new NativeErrorImp(&m_globalExec, funcProto, m_TypeErrorPrototype);
    m_UriError = new NativeErrorImp(&m_globalExec, funcProto, m_UriErrorPrototype);

    // ECMA 15.3.4.1
    funcProto->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_Function, DontEnum);

    putNamedConstructor("Object", m_Object);
    putNamedConstructor("Function", m_Function);
    putNamedConstructor("Array", m_Array);
    putNamedConstructor("Boolean", m_Boolean);
    putNamedConstructor("String", m_String);
    putNamedConstructor("Number", m_Number);
    putNamedConstructor("Date", m_Date);
    putNamedConstructor("RegExp", m_RegExp);
    putNamedConstructor("Error", m_Error);
    putNamedConstructor("EvalError",m_EvalError);
    putNamedConstructor("RangeError",m_RangeError);
    putNamedConstructor("ReferenceError",m_ReferenceError);
    putNamedConstructor("SyntaxError",m_SyntaxError);
    putNamedConstructor("TypeError",m_TypeError);
    putNamedConstructor("URIError",m_UriError);
    
    // Set the constructorPropertyName property of all builtin constructors
    objProto->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_Object, DontEnum);
    funcProto->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_Function, DontEnum);
    arrayProto->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_Array, DontEnum);
    booleanProto->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_Boolean, DontEnum);
    stringProto->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_String, DontEnum);
    numberProto->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_Number, DontEnum);
    dateProto->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_Date, DontEnum);
    regexpProto->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_RegExp, DontEnum);
    errorProto->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_Error, DontEnum);
    m_EvalErrorPrototype->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_EvalError, DontEnum);
    m_RangeErrorPrototype->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_RangeError, DontEnum);
    m_ReferenceErrorPrototype->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_ReferenceError, DontEnum);
    m_SyntaxErrorPrototype->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_SyntaxError, DontEnum);
    m_TypeErrorPrototype->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_TypeError, DontEnum);
    m_UriErrorPrototype->put(&m_globalExec, m_globalExec.propertyNames().constructor, m_UriError, DontEnum);

    // built-in values
    m_globalObject->put(&m_globalExec, "NaN",        jsNaN(), DontEnum|DontDelete|ReadOnly);
    m_globalObject->put(&m_globalExec, "Infinity",   jsNumber(Inf), DontEnum|DontDelete|ReadOnly);
    m_globalObject->put(&m_globalExec, "undefined",  jsUndefined(), DontEnum|DontDelete|ReadOnly);

    // built-in functions
    m_globalObject->putDirectFunction(new GlobalFuncImp(&m_globalExec, funcProto, GlobalFuncImp::Eval, 1, "eval"), DontEnum);
    m_globalObject->putDirectFunction(new GlobalFuncImp(&m_globalExec, funcProto, GlobalFuncImp::ParseInt, 2, "parseInt"), DontEnum);
    m_globalObject->putDirectFunction(new GlobalFuncImp(&m_globalExec, funcProto, GlobalFuncImp::ParseFloat, 1, "parseFloat"), DontEnum);
    m_globalObject->putDirectFunction(new GlobalFuncImp(&m_globalExec, funcProto, GlobalFuncImp::IsNaN, 1, "isNaN"), DontEnum);
    m_globalObject->putDirectFunction(new GlobalFuncImp(&m_globalExec, funcProto, GlobalFuncImp::IsFinite, 1, "isFinite"), DontEnum);
    m_globalObject->putDirectFunction(new GlobalFuncImp(&m_globalExec, funcProto, GlobalFuncImp::Escape, 1, "escape"), DontEnum);
    m_globalObject->putDirectFunction(new GlobalFuncImp(&m_globalExec, funcProto, GlobalFuncImp::UnEscape, 1, "unescape"), DontEnum);
    m_globalObject->putDirectFunction(new GlobalFuncImp(&m_globalExec, funcProto, GlobalFuncImp::DecodeURI, 1, "decodeURI"), DontEnum);
    m_globalObject->putDirectFunction(new GlobalFuncImp(&m_globalExec, funcProto, GlobalFuncImp::DecodeURIComponent, 1, "decodeURIComponent"), DontEnum);
    m_globalObject->putDirectFunction(new GlobalFuncImp(&m_globalExec, funcProto, GlobalFuncImp::EncodeURI, 1, "encodeURI"), DontEnum);
    m_globalObject->putDirectFunction(new GlobalFuncImp(&m_globalExec, funcProto, GlobalFuncImp::EncodeURIComponent, 1, "encodeURIComponent"), DontEnum);
#ifndef NDEBUG
    m_globalObject->putDirectFunction(new GlobalFuncImp(&m_globalExec, funcProto, GlobalFuncImp::KJSPrint, 1, "kjsprint"), DontEnum);
#endif

    // built-in objects
    m_globalObject->put(&m_globalExec, "Math", new MathObjectImp(&m_globalExec, objProto), DontEnum);
    m_globalObject->put(&m_globalExec, "JSON", new JSONObjectImp(&m_globalExec, objProto), DontEnum);
}

ExecState* Interpreter::globalExec()
{
  return &m_globalExec;
}

void Interpreter::setGlobalPackage(Package* p)
{
    globPkg = p;
}

Package* Interpreter::globalPackage()
{
    return globPkg;
}

Completion Interpreter::checkSyntax(const UString& sourceURL, int startingLineNumber, const UString& code)
{
    return checkSyntax(sourceURL, startingLineNumber, code.data(), code.size());
}

Completion Interpreter::checkSyntax(const UString& sourceURL, int startingLineNumber, const UChar* code, int codeLength)
{
    JSLock lock;

    int errLine;
    UString errMsg;
    RefPtr<ProgramNode> progNode = parser().parseProgram(sourceURL, startingLineNumber, code, codeLength, 0, &errLine, &errMsg);
    if (!progNode)
        return Completion(Throw, Error::create(&m_globalExec, SyntaxError, errMsg, errLine, 0, sourceURL));
    return Completion(Normal);
}

Completion Interpreter::evaluate(const UString& sourceURL, int startingLineNumber, const UString& code, JSValue* thisV)
{
    return evaluate(sourceURL, startingLineNumber, code.data(), code.size(), thisV);
}

Completion Interpreter::evaluate(const UString& sourceURL, int startingLineNumber, const UChar* code, int codeLength, JSValue* thisV)
{
    JSLock lock;

    // prevent against infinite recursion
    if (m_recursion >= 20)
        return Completion(Throw, Error::create(&m_globalExec, GeneralError, "Recursion too deep"));

    // parse the source code
    int sourceId;
    int errLine;
    UString errMsg;
    RefPtr<ProgramNode> progNode = parser().parseProgram(sourceURL, startingLineNumber, code, codeLength, &sourceId, &errLine, &errMsg);

    // notify debugger that source has been parsed
    if (m_debugger) {
        m_debugger->reportSourceParsed(&m_globalExec, progNode.get(), sourceId, sourceURL, 
                      UString(code, codeLength), startingLineNumber, errLine, errMsg);
    }

    // no program node means a syntax error occurred
    if (!progNode) {
        Completion res(Throw, Error::create(&m_globalExec, SyntaxError, errMsg, errLine, sourceId, sourceURL));
        if (m_debugger)
            m_debugger->reportException(&m_globalExec, res.value());

        if (shouldPrintExceptions())
            printException(res, sourceURL);
        return res;
    }

    m_globalExec.clearException();

    m_recursion++;

    JSGlobalObject* globalObj = m_globalObject;
    JSObject*       thisObj   = globalObj;

    // "this" must be an object... use same rules as Function.prototype.apply()
    if (thisV && !thisV->isUndefinedOrNull())
        thisObj = thisV->toObject(&m_globalExec);

    Completion res;
    if (m_globalExec.hadException())
        // the thisV->toObject() conversion above might have thrown an exception - if so, propagate it
        res = Completion(Throw, m_globalExec.exception());
    else {
        // execute the code
        InterpreterExecState newExec(this, globalObj, thisObj, progNode.get());

        if (m_debugger && !m_debugger->enterContext(&newExec, sourceId, startingLineNumber, 0, List::empty())) {
            // debugger requested we stop execution.
            m_debugger->imp()->abort();
            return Completion(Break);
        }

        progNode->processDecls(&newExec);
        res = progNode->execute(&newExec);

        if (m_debugger && !m_debugger->exitContext(&newExec, sourceId, startingLineNumber, 0)) {
            // debugger requested we stop execution.
            m_debugger->imp()->abort();
            return Completion(Break);
        }
    }

    m_recursion--;

    if (shouldPrintExceptions() && res.complType() == Throw)
        printException(res, sourceURL);

    return res;
}

bool Interpreter::normalizeCode(const UString& codeIn, UString* codeOut,
                                int* errLine, UString* errMsg)
{
    assert(codeOut);
    RefPtr<ProgramNode> progNode = parser().parseProgram("", // sourceURL
                                                         0, // line
                                                         codeIn.data(),
                                                         codeIn.size(),
                                                         0, // &sourceId
                                                         errLine, errMsg);
    if (progNode) {
        *codeOut = progNode->toString();
        return true;
    } else {
        return false;
    }
}

JSObject *Interpreter::builtinObject() const
{
  return m_Object;
}

JSObject *Interpreter::builtinFunction() const
{
  return m_Function;
}

JSObject *Interpreter::builtinArray() const
{
  return m_Array;
}

JSObject *Interpreter::builtinBoolean() const
{
  return m_Boolean;
}

JSObject *Interpreter::builtinString() const
{
  return m_String;
}

JSObject *Interpreter::builtinNumber() const
{
  return m_Number;
}

JSObject *Interpreter::builtinDate() const
{
  return m_Date;
}

JSObject *Interpreter::builtinRegExp() const
{
  return m_RegExp;
}

JSObject *Interpreter::builtinError() const
{
  return m_Error;
}

JSObject *Interpreter::builtinObjectPrototype() const
{
  return m_ObjectPrototype;
}

JSObject *Interpreter::builtinFunctionPrototype() const
{
  return m_FunctionPrototype;
}

JSObject *Interpreter::builtinArrayPrototype() const
{
  return m_ArrayPrototype;
}

JSObject *Interpreter::builtinBooleanPrototype() const
{
  return m_BooleanPrototype;
}

JSObject *Interpreter::builtinStringPrototype() const
{
  return m_StringPrototype;
}

JSObject *Interpreter::builtinNumberPrototype() const
{
  return m_NumberPrototype;
}

JSObject *Interpreter::builtinDatePrototype() const
{
  return m_DatePrototype;
}

JSObject *Interpreter::builtinRegExpPrototype() const
{
  return m_RegExpPrototype;
}

JSObject *Interpreter::builtinErrorPrototype() const
{
  return m_ErrorPrototype;
}

JSObject *Interpreter::builtinEvalError() const
{
  return m_EvalError;
}

JSObject *Interpreter::builtinRangeError() const
{
  return m_RangeError;
}

JSObject *Interpreter::builtinReferenceError() const
{
  return m_ReferenceError;
}

JSObject *Interpreter::builtinSyntaxError() const
{
  return m_SyntaxError;
}

JSObject *Interpreter::builtinTypeError() const
{
  return m_TypeError;
}

JSObject *Interpreter::builtinURIError() const
{
  return m_UriError;
}

JSObject *Interpreter::builtinEvalErrorPrototype() const
{
  return m_EvalErrorPrototype;
}

JSObject *Interpreter::builtinRangeErrorPrototype() const
{
  return m_RangeErrorPrototype;
}

JSObject *Interpreter::builtinReferenceErrorPrototype() const
{
  return m_ReferenceErrorPrototype;
}

JSObject *Interpreter::builtinSyntaxErrorPrototype() const
{
  return m_SyntaxErrorPrototype;
}

JSObject *Interpreter::builtinTypeErrorPrototype() const
{
  return m_TypeErrorPrototype;
}

JSObject *Interpreter::builtinURIErrorPrototype() const
{
  return m_UriErrorPrototype;
}

bool Interpreter::collect()
{
  return Collector::collect();
}

void Interpreter::mark(bool)
{
    if (m_execState)
        m_execState->mark();
    if (m_globalObject && !m_globalObject->marked())
        m_globalObject->mark();
    if (m_globalExec.exception() && !m_globalExec.exception()->marked())
        m_globalExec.exception()->mark();

    // Do not let cached activations survive the GC; as they have an unfortunate
    // tendenacy to pin blocks, increasing their number and hence spreading out
    // the objects somewhat
    m_numCachedActivations = 0;
}

void Interpreter::markSourceCachedObjects()
{
    markInternedStringsTable();
}

#ifdef KJS_DEBUG_MEM
void Interpreter::finalCheck()
{
  fprintf(stderr,"Interpreter::finalCheck()\n");
  Collector::collect();

//  Node::finalCheck();
  Collector::finalCheck();
}
#endif

static bool printExceptions = false;

bool Interpreter::shouldPrintExceptions()
{
  return printExceptions;
}

void Interpreter::setShouldPrintExceptions(bool print)
{
  printExceptions = print;
}

void Interpreter::printException(const Completion& c, const UString& sourceURL)
{
    JSLock lock;
    ExecState* exec = globalExec();
    CString f = sourceURL.UTF8String();
    CString message = c.value()->toObject(exec)->toString(exec).UTF8String();
    int line = c.value()->toObject(exec)->get(exec, "line")->toUInt32(exec);
#if PLATFORM(WIN_OS)
    printf("%s line %d: %s\n", f.c_str(), line, message.c_str());
#else
    printf("[%d] %s line %d: %s\n", getpid(), f.c_str(), line, message.c_str());
#endif
}

// bindings are OS X WebKit-only for now
#if USE(BINDINGS)
void *Interpreter::createLanguageInstanceForValue(ExecState *exec, int language, JSObject *value, const Bindings::RootObject *origin, const Bindings::RootObject *current)
{
    return Bindings::Instance::createLanguageInstanceForValue (exec, (Bindings::Instance::BindingLanguage)language, value, origin, current);
}
#endif

void Interpreter::saveBuiltins (SavedBuiltins& builtins) const
{
    if (!builtins._internal)
        builtins._internal = new SavedBuiltinsInternal;

    builtins._internal->m_Object = m_Object;
    builtins._internal->m_Function = m_Function;
    builtins._internal->m_Array = m_Array;
    builtins._internal->m_Boolean = m_Boolean;
    builtins._internal->m_String = m_String;
    builtins._internal->m_Number = m_Number;
    builtins._internal->m_Date = m_Date;
    builtins._internal->m_RegExp = m_RegExp;
    builtins._internal->m_Error = m_Error;

    builtins._internal->m_ObjectPrototype = m_ObjectPrototype;
    builtins._internal->m_FunctionPrototype = m_FunctionPrototype;
    builtins._internal->m_ArrayPrototype = m_ArrayPrototype;
    builtins._internal->m_BooleanPrototype = m_BooleanPrototype;
    builtins._internal->m_StringPrototype = m_StringPrototype;
    builtins._internal->m_NumberPrototype = m_NumberPrototype;
    builtins._internal->m_DatePrototype = m_DatePrototype;
    builtins._internal->m_RegExpPrototype = m_RegExpPrototype;
    builtins._internal->m_ErrorPrototype = m_ErrorPrototype;

    builtins._internal->m_EvalError = m_EvalError;
    builtins._internal->m_RangeError = m_RangeError;
    builtins._internal->m_ReferenceError = m_ReferenceError;
    builtins._internal->m_SyntaxError = m_SyntaxError;
    builtins._internal->m_TypeError = m_TypeError;
    builtins._internal->m_UriError = m_UriError;

    builtins._internal->m_EvalErrorPrototype = m_EvalErrorPrototype;
    builtins._internal->m_RangeErrorPrototype = m_RangeErrorPrototype;
    builtins._internal->m_ReferenceErrorPrototype = m_ReferenceErrorPrototype;
    builtins._internal->m_SyntaxErrorPrototype = m_SyntaxErrorPrototype;
    builtins._internal->m_TypeErrorPrototype = m_TypeErrorPrototype;
    builtins._internal->m_UriErrorPrototype = m_UriErrorPrototype;
}

void Interpreter::restoreBuiltins (const SavedBuiltins& builtins)
{
    if (!builtins._internal)
        return;

    m_Object = builtins._internal->m_Object;
    m_Function = builtins._internal->m_Function;
    m_Array = builtins._internal->m_Array;
    m_Boolean = builtins._internal->m_Boolean;
    m_String = builtins._internal->m_String;
    m_Number = builtins._internal->m_Number;
    m_Date = builtins._internal->m_Date;
    m_RegExp = builtins._internal->m_RegExp;
    m_Error = builtins._internal->m_Error;

    m_ObjectPrototype = builtins._internal->m_ObjectPrototype;
    m_FunctionPrototype = builtins._internal->m_FunctionPrototype;
    m_ArrayPrototype = builtins._internal->m_ArrayPrototype;
    m_BooleanPrototype = builtins._internal->m_BooleanPrototype;
    m_StringPrototype = builtins._internal->m_StringPrototype;
    m_NumberPrototype = builtins._internal->m_NumberPrototype;
    m_DatePrototype = builtins._internal->m_DatePrototype;
    m_RegExpPrototype = builtins._internal->m_RegExpPrototype;
    m_ErrorPrototype = builtins._internal->m_ErrorPrototype;

    m_EvalError = builtins._internal->m_EvalError;
    m_RangeError = builtins._internal->m_RangeError;
    m_ReferenceError = builtins._internal->m_ReferenceError;
    m_SyntaxError = builtins._internal->m_SyntaxError;
    m_TypeError = builtins._internal->m_TypeError;
    m_UriError = builtins._internal->m_UriError;

    m_EvalErrorPrototype = builtins._internal->m_EvalErrorPrototype;
    m_RangeErrorPrototype = builtins._internal->m_RangeErrorPrototype;
    m_ReferenceErrorPrototype = builtins._internal->m_ReferenceErrorPrototype;
    m_SyntaxErrorPrototype = builtins._internal->m_SyntaxErrorPrototype;
    m_TypeErrorPrototype = builtins._internal->m_TypeErrorPrototype;
    m_UriErrorPrototype = builtins._internal->m_UriErrorPrototype;
}

void Interpreter::startTimeoutCheck()
{
    if (!m_timeoutChecker)
        m_timeoutChecker = new TimeoutChecker;

    m_timeoutChecker->startTimeoutCheck(this);
}

void Interpreter::stopTimeoutCheck()
{
    ASSERT(m_timeoutChecker);

    m_timeoutChecker->stopTimeoutCheck(this);
}

void Interpreter::restartTimeoutCheck()
{
    if (!m_timeoutChecker || !m_startTimeoutCheckCount)
        return;

    m_timedOut = false;
    m_timeoutChecker->stopTimeoutCheck(this);
    m_timeoutChecker->startTimeoutCheck(this);
}

void Interpreter::pauseTimeoutCheck()
{
    ASSERT(m_timeoutChecker);

    m_timeoutChecker->pauseTimeoutCheck(this);
}

void Interpreter::resumeTimeoutCheck()
{
    ASSERT(m_timeoutChecker);

    m_timeoutChecker->resumeTimeoutCheck(this);
}

bool Interpreter::handleTimeout()
{
    m_timedOut = false;

    pauseTimeoutCheck();
    bool retval = shouldInterruptScript();
    resumeTimeoutCheck();

    return retval;
}

Interpreter::InternedStringsTable* Interpreter::s_internedStrings;

void Interpreter::initInternedStringsTable()
{
    if (!s_internedStrings)
        s_internedStrings = new InternedStringsTable();
}

StringImp* Interpreter::internString(const UString& literal)
{
    InternedStringsTable::iterator i = s_internedStrings->find(literal.rep());

    if (i == s_internedStrings->end()) {
        // Need to add. Note: we can't use ->add() above to avoid a double-hash
        // as creation of a StringImp may cause a GC, which in turn may
        // rearrange the hashtable, invalidating the iterator.
        StringImp* si = static_cast<StringImp*>(jsOwnedString(literal));
        s_internedStrings->add(literal.rep(), std::make_pair(si, 1));
        return si;
    } else {
        ++i.values()->second; // just bump the ref count
        return i.values()->first;
    }
}

void Interpreter::releaseInternedString(const UString& literal)
{
    InternedStringsTable::iterator i = s_internedStrings->find(literal.rep());

    --i.values()->second;
    if (i.values()->second == 0)
        s_internedStrings->remove(i);
}
    
void Interpreter::markInternedStringsTable()
{
    for (InternedStringsTable::iterator i = s_internedStrings->begin();
        i != s_internedStrings->end(); ++i) {
        // Note: the StringImp* may be null here if we got called in the middle 
        // of internString.
        if (i.values()->first && !i.values()->first->marked())
            i.values()->first->mark();
    }
}
    
SavedBuiltins::SavedBuiltins() :
  _internal(0)
{
}

SavedBuiltins::~SavedBuiltins()
{
  delete _internal;
}

UnicodeSupport::UnicodeSupport()
{
}

void UnicodeSupport::setIdentStartChecker(bool (*f)(int c))
{
  Lexer::setIdentStartChecker(f);
}

void UnicodeSupport::setIdentPartChecker(bool (*f)(int c))
{
  Lexer::setIdentPartChecker(f);
}

void UnicodeSupport::setToLowerFunction(StringConversionFunction f)
{
  StringProtoFunc::setToLowerFunction(f);
}

void UnicodeSupport::setToUpperFunction(StringConversionFunction f)
{
  StringProtoFunc::setToUpperFunction(f);
}

}

// kate: indent-width 2; replace-tabs on; tab-width 4; space-indent on;
