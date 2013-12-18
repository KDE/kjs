/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003 Apple Computer, Inc.
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

#ifndef _KJS_INTERPRETER_H_
#define _KJS_INTERPRETER_H_

#include "ExecState.h"
#include "protect.h"
#include "value.h"
#include "types.h"
#include <wtf/HashMap.h>

namespace KJS {
  class Debugger;
  class SavedBuiltins;
  class TimeoutChecker;
  class Package;
  class ActivationImp;
  class JSGlobalObject;
  class StringImp;

#if USE(BINDINGS)
  namespace Bindings {
    class RootObject;
  }
#endif

  /**
   * Interpreter objects can be used to evaluate ECMAScript code. Each
   * interpreter has a global object which is used for the purposes of code
   * evaluation, and also provides access to built-in properties such as
   * " Object" and "Number".
   */
  class KJS_EXPORT Interpreter {
      friend class Collector;
      friend class TimeoutChecker;
  public:
    /**
     * Creates a new interpreter. The supplied object will be used as the global
     * object for all scripts executed with this interpreter. During
     * construction, all the standard properties such as "Object" and "Number"
     * will be added to the global object.
     *
     * Note: You should not use the same global object for multiple
     * interpreters.
     *
     * This is due do the fact that the built-in properties are set in the
     * constructor, and if these objects have been modified from another
     * interpreter (e.g. a script modifying String.prototype), the changes will
     * be overridden.
     *
     * @param globalObject The object to use as the global object for this interpreter
     */
    Interpreter(JSGlobalObject* globalObject);
    /**
     * Creates a new interpreter. A global object will be created and
     * initialized with the standard global properties.
     */
    Interpreter();

    /**
     * Returns the object that is used as the global object during all script
     * execution performed by this interpreter
     */
    JSGlobalObject* globalObject() const;
    void initGlobalObject();

    /**
     * Returns the execution state object which can be used to execute
     * scripts using this interpreter at a the "global" level, i.e. one
     * with a execution context that has the global object as the "this"
     * value, and who's scope chain contains only the global object.
     *
     * Note: this pointer remains constant for the life of the interpreter
     * and should not be manually deleted.
     *
     * @return The interpreter global execution state object
     */
    virtual ExecState *globalExec();

    /**
     * Sets the package instance that will be used to resolve the
     * first level of identifiers of import statements.
     *
     * If no package is set which will make any "import" script
     * statement fail with an error. This is the default in e.g.  a
     * Web browser where package imports should be disabled for
     * security reasons.
     */
    void setGlobalPackage(Package* p);

    /**
     * Returns the package that was installed to handle top level
     * package requests. Returns 0, the default, when no package was
     * set.
     *
     * @return The global package
     */
    Package* globalPackage();

    /**
     * Parses the supplied ECMAScript code and checks for syntax errors.
     *
     * @param code The code to check
     * @param sourceURL A URL denoting the origin of the code
     * @param startingLineNumber The line offset within an embedding context
     * @return A normal completion if there were no syntax errors in the code, 
     * otherwise a throw completion with the syntax error as its value.
     */
    Completion checkSyntax(const UString& sourceURL, int startingLineNumber, const UString& code);
    Completion checkSyntax(const UString& sourceURL, int startingLineNumber, const UChar* code, int codeLength);

    /**
     * Evaluates the supplied ECMAScript code.
     *
     * Since this method returns a Completion, you should check the type of
     * completion to detect an error or before attempting to access the returned
     * value. For example, if an error occurs during script execution and is not
     * caught by the script, the completion type will be Throw.
     *
     * If the supplied code is invalid, a SyntaxError will be thrown.
     *
     * @param sourceURL A URL denoting the origin of the code
     * @param startingLineNumber The line offset within an embedding context
     * @param code The code to evaluate
     * @param codeLength The length of the code to evaluate
     * @param thisV The value to pass in as the "this" value for the script
     * execution. This should either be jsNull() or an Object.
     * @return A completion object representing the result of the execution.
     */
    Completion evaluate(const UString& sourceURL, int startingLineNumber, const UChar* code, int codeLength, JSValue* thisV = 0);
    Completion evaluate(const UString& sourceURL, int startingLineNumber, const UString& code, JSValue* thisV = 0);

    /**
     * Pretty-prints the supplied ECMAScript code after checking it
     * for syntax errors.
     *
     * @param sourceURL A URL denoting the origin of the code
     * @param startingLineNumber The line offset within an embedding context
     * @param codeIn The code to check
     * @param codeIn Pointer to string that will contain reformatted code
     *        upon successful parsing.
     * @return A normal completion if there were no syntax errors in the code, 
     * otherwise a throw completion with the syntax error as its value.
     */
    static bool normalizeCode(const UString& codeIn, UString* codeOut,
                              int* errLine = 0, UString* errMsg = 0);

    /**
     * Returns the builtin "Object" object. This is the object that was set
     * as a property of the global object during construction; if the property
     * is replaced by script code, this method will still return the original
     * object.
     *
     * @return The builtin "Object" object
     */
    JSObject *builtinObject() const;

    /**
     * Returns the builtin "Function" object.
     */
    JSObject *builtinFunction() const;

    /**
     * Returns the builtin "Array" object.
     */
    JSObject *builtinArray() const;

    /**
     * Returns the builtin "Boolean" object.
     */
    JSObject *builtinBoolean() const;

    /**
     * Returns the builtin "String" object.
     */
    JSObject *builtinString() const;

    /**
     * Returns the builtin "Number" object.
     */
    JSObject *builtinNumber() const;

    /**
     * Returns the builtin "Date" object.
     */
    JSObject *builtinDate() const;

    /**
     * Returns the builtin "RegExp" object.
     */
    JSObject *builtinRegExp() const;

    /**
     * Returns the builtin "Error" object.
     */
    JSObject *builtinError() const;

    /**
     * Returns the builtin "Object.prototype" object.
     */
    JSObject *builtinObjectPrototype() const;

    /**
     * Returns the builtin "Function.prototype" object.
     */
    JSObject *builtinFunctionPrototype() const;

    /**
     * Returns the builtin "Array.prototype" object.
     */
    JSObject *builtinArrayPrototype() const;

    /**
     * Returns the builtin "Boolean.prototype" object.
     */
    JSObject *builtinBooleanPrototype() const;

    /**
     * Returns the builtin "String.prototype" object.
     */
    JSObject *builtinStringPrototype() const;

    /**
     * Returns the builtin "Number.prototype" object.
     */
    JSObject *builtinNumberPrototype() const;

    /**
     * Returns the builtin "Date.prototype" object.
     */
    JSObject *builtinDatePrototype() const;

    /**
     * Returns the builtin "RegExp.prototype" object.
     */
    JSObject *builtinRegExpPrototype() const;

    /**
     * Returns the builtin "Error.prototype" object.
     */
    JSObject *builtinErrorPrototype() const;

    /**
     * The initial value of "Error" global property
     */
    JSObject *builtinEvalError() const;
    JSObject *builtinRangeError() const;
    JSObject *builtinReferenceError() const;
    JSObject *builtinSyntaxError() const;
    JSObject *builtinTypeError() const;
    JSObject *builtinURIError() const;

    JSObject *builtinEvalErrorPrototype() const;
    JSObject *builtinRangeErrorPrototype() const;
    JSObject *builtinReferenceErrorPrototype() const;
    JSObject *builtinSyntaxErrorPrototype() const;
    JSObject *builtinTypeErrorPrototype() const;
    JSObject *builtinURIErrorPrototype() const;

    enum CompatMode { NativeMode, IECompat, NetscapeCompat };
    /**
     * Call this to enable a compatibility mode with another browser.
     * (by default konqueror is in "native mode").
     * Currently, in KJS, this only changes the behavior of Date::getYear()
     * which returns the full year under IE.
     */
    void setCompatMode(CompatMode mode) { m_compatMode = mode; }
    CompatMode compatMode() const { return m_compatMode; }

    /**
     * Run the garbage collection. Returns true when at least one object
     * was collected; false otherwise.
     */
    static bool collect();

    /**
     * Called during the mark phase of the garbage collector. Subclasses
     * implementing custom mark methods must make sure to chain to this one.
     */
    virtual void mark(bool currentThreadIsMainThread);
    
    /**
     * This marks all GC heap resources stored as optimizations;
     * and which have their lifetime managed by the appropriate AST.
     * It's static since code can survive the interpreter by a bit.
     */
    static void markSourceCachedObjects();

    /**
     * Provides a way to distinguish derived classes.
     * Only useful if you reimplement Interpreter and if different kind of
     * interpreters are created in the same process.
     * The base class returns 0, the ECMA-bindings interpreter returns 1.
     */
    virtual int rtti() { return 0; }

    static bool shouldPrintExceptions();
    static void setShouldPrintExceptions(bool);

    void saveBuiltins (SavedBuiltins&) const;
    void restoreBuiltins (const SavedBuiltins&);

    /**
     * Determine if the it is 'safe' to execute code in the target interpreter from an
     * object that originated in this interpreter.  This check is used to enforce WebCore
     * cross frame security rules.  In particular, attempts to access 'bound' objects are
     * not allowed unless isSafeScript returns true.
     */
    virtual bool isSafeScript(const Interpreter*) { return true; }

#if USE(BINDINGS)
    virtual void *createLanguageInstanceForValue(ExecState*, int language, JSObject* value, const Bindings::RootObject* origin, const Bindings::RootObject* current);
#endif

    // Chained list of interpreters (ring)
    static Interpreter* firstInterpreter() { return s_hook; }
    Interpreter* nextInterpreter() const { return next; }
    Interpreter* prevInterpreter() const { return prev; }

    Debugger* debugger() const { return m_debugger; }
    void setDebugger(Debugger* d) { m_debugger = d; }

    void setExecState(ExecState* e) { m_execState = e; }

    // Note: may be 0, if in globalExec
    ExecState* execState() { return m_execState ? m_execState : &m_globalExec; }

    void setTimeoutTime(unsigned timeoutTime) { m_timeoutTime = timeoutTime; }

    void startTimeoutCheck();
    void stopTimeoutCheck();

    // Resets the timer to full time if it's running
    void restartTimeoutCheck();

    void pauseTimeoutCheck();
    void resumeTimeoutCheck();

    bool checkTimeout();

    void ref() { ++m_refCount; }
    void deref() { if (--m_refCount <= 0) delete this; }
    int refCount() const { return m_refCount; }

    unsigned char* stackAlloc(size_t size) {
        unsigned char* nextPtr = stackPtr + size;
        if (nextPtr <= stackEnd) {
            unsigned char* toRet = stackPtr;
            stackPtr = nextPtr;
            return toRet;
        }
        return extendStack(size);
    }

    void stackFree(size_t size) { stackPtr-= size; } // ### shrink it?

    ActivationImp* getRecycledActivation() {
        ActivationImp* out = 0;
        if (m_numCachedActivations) {
            m_numCachedActivations--;
            out = m_cachedActivations[m_numCachedActivations];
        }
        return out;
    }

    void recycleActivation(ActivationImp* act);
    
    // Global string table management. This is used from StringNode
    // to cache StringImp's for string literals. We keep refcounts
    // to permit multiple ones to use the same value.
    static StringImp* internString(const UString& literal);
    static void releaseInternedString(const UString& literal);

    typedef WTF::HashMap<UString::Rep*, std::pair<KJS::StringImp*, int> > InternedStringsTable;
private:    
    static void markInternedStringsTable();
    
    // This creates a table if needed
    static void initInternedStringsTable();
    
    static InternedStringsTable* s_internedStrings;

protected:
    virtual ~Interpreter(); // only deref should delete us
    virtual bool shouldInterruptScript() const { return true; }

    long m_timeoutTime;

private:
    bool handleTimeout();
    void init();
    void printException(const Completion& c, const UString& sourceURL);

    /**
     * This constructor is not implemented, in order to prevent
     * copy-construction of Interpreter objects. You should always pass around
     * pointers to an interpreter instance instead.
     */
    Interpreter(const Interpreter&);

    /**
     * This operator is not implemented, in order to prevent assignment of
     * Interpreter objects. You should always pass around pointers to an
     * interpreter instance instead.
     */
    Interpreter operator=(const Interpreter&);

    int m_refCount;

    JSGlobalObject* m_globalObject;
    GlobalExecState m_globalExec;
    Package *globPkg;

    // Execution stack stuff for this interpreter.
    unsigned char* stackBase; // lowest address in the array
    unsigned char* stackPtr;  // current top/next to allocate
    unsigned char* stackEnd;  // last address in the stack
    unsigned char* extendStack(size_t needed);

    // A list of cached activations
    enum {MaxCachedActivations = 32};

    ActivationImp* m_cachedActivations[MaxCachedActivations];
    int            m_numCachedActivations;

    // Chained list of interpreters (ring) - for collector
    static Interpreter* s_hook;
    Interpreter *next, *prev;

    int m_recursion;

    Debugger* m_debugger;
    ExecState* m_execState;
    CompatMode m_compatMode;

    TimeoutChecker* m_timeoutChecker;
    bool m_timedOut;

    unsigned m_startTimeoutCheckCount;
    unsigned m_pauseTimeoutCheckCount;
    
    // Helper for setting constructors, making sure their function names are OK
    void putNamedConstructor(const char* name, JSObject* value);

    ProtectedPtr<JSObject> m_Object;
    ProtectedPtr<JSObject> m_Function;
    ProtectedPtr<JSObject> m_Array;
    ProtectedPtr<JSObject> m_Boolean;
    ProtectedPtr<JSObject> m_String;
    ProtectedPtr<JSObject> m_Number;
    ProtectedPtr<JSObject> m_Date;
    ProtectedPtr<JSObject> m_RegExp;
    ProtectedPtr<JSObject> m_Error;

    ProtectedPtr<JSObject> m_ObjectPrototype;
    ProtectedPtr<JSObject> m_FunctionPrototype;
    ProtectedPtr<JSObject> m_ArrayPrototype;
    ProtectedPtr<JSObject> m_BooleanPrototype;
    ProtectedPtr<JSObject> m_StringPrototype;
    ProtectedPtr<JSObject> m_NumberPrototype;
    ProtectedPtr<JSObject> m_DatePrototype;
    ProtectedPtr<JSObject> m_RegExpPrototype;
    ProtectedPtr<JSObject> m_ErrorPrototype;

    ProtectedPtr<JSObject> m_EvalError;
    ProtectedPtr<JSObject> m_RangeError;
    ProtectedPtr<JSObject> m_ReferenceError;
    ProtectedPtr<JSObject> m_SyntaxError;
    ProtectedPtr<JSObject> m_TypeError;
    ProtectedPtr<JSObject> m_UriError;

    ProtectedPtr<JSObject> m_EvalErrorPrototype;
    ProtectedPtr<JSObject> m_RangeErrorPrototype;
    ProtectedPtr<JSObject> m_ReferenceErrorPrototype;
    ProtectedPtr<JSObject> m_SyntaxErrorPrototype;
    ProtectedPtr<JSObject> m_TypeErrorPrototype;
    ProtectedPtr<JSObject> m_UriErrorPrototype;
  };

  inline bool Interpreter::checkTimeout()
  {
    if (!m_timedOut)
      return false;

    return handleTimeout();
  }

  /**
   * Interface to set enhanced Unicode support functions. By default
   * the interpreter will use the standard C library functions.
   *
   * @internal
   */
  class KJS_EXPORT UnicodeSupport
  {
  public:
    UnicodeSupport();

    typedef bool (*CharCategoryFunction)(int c);
    static void setIdentStartChecker(CharCategoryFunction f);
    static void setIdentPartChecker(CharCategoryFunction f);

    typedef int (*StringConversionFunction)(uint16_t* str, int strLength,
					    uint16_t*& destIfNeeded);
    static void setToLowerFunction(StringConversionFunction f);
    static void setToUpperFunction(StringConversionFunction f);
  };

  /**
   * Define a Qt-based version of the Unicode support functions.
   *
   * @internal
   */
#define KJS_QT_UNICODE_IMPL \
namespace KJS { \
  static bool qtIdentStart(int c) { if (c & 0xffff0000) return false; QChar::Category cat = QChar((unsigned short)c).category(); return cat == QChar::Letter_Uppercase || cat == QChar::Letter_Lowercase || cat == QChar::Letter_Titlecase || cat == QChar::Letter_Modifier || cat == QChar::Letter_Other || c == '$' || c == '_'; } \
  static bool qtIdentPart(int c) { if (c & 0xffff0000) return false; QChar::Category cat = QChar((unsigned short)c).category(); return cat == QChar::Letter_Uppercase || cat == QChar::Letter_Lowercase || cat == QChar::Letter_Titlecase || cat == QChar::Letter_Modifier || cat == QChar::Letter_Other || cat == QChar::Mark_NonSpacing || cat == QChar::Mark_SpacingCombining || cat == QChar::Number_DecimalDigit || cat == QChar::Punctuation_Connector || c == '$' || c == '_'; } \
  static int qtToLower(uint16_t* str, int strLength, uint16_t*& destIfNeeded) { \
      destIfNeeded = 0; \
      for (int i = 0; i < strLength; ++i) \
        str[i] = QChar(str[i]).toLower().unicode(); \
      return strLength; } \
  static int qtToUpper(uint16_t* str, int strLength, uint16_t*& destIfNeeded) { \
      destIfNeeded = 0; \
      for (int i = 0; i < strLength; ++i) \
        str[i] = QChar(str[i]).toUpper().unicode(); \
      return strLength; } \
}

  /**
   * Set the Qt-based version of the Unicode support functions.
   *
   * @internal
   */
#define KJS_QT_UNICODE_SET \
  { KJS::UnicodeSupport::setIdentStartChecker(KJS::qtIdentStart); \
    KJS::UnicodeSupport::setIdentPartChecker(KJS::qtIdentPart); \
    KJS::UnicodeSupport::setToLowerFunction(KJS::qtToLower); \
    KJS::UnicodeSupport::setToUpperFunction(KJS::qtToUpper); }

} // namespace

#endif // _KJS_INTERPRETER_H_
