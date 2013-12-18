// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
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

#ifndef _KJSDEBUGGER_H_
#define _KJSDEBUGGER_H_

#include "global.h"
#include <wtf/HashMap.h>
#include "protect.h"

namespace KJS {

  class DebuggerImp;
  class Interpreter;
  class ExecState;
  class JSObject;
  class JSValue;
  class UString;
  class List;
  class FunctionBodyNode;

  /**
   * @internal
   *
   * Provides an interface which receives notification about various
   * script-execution related events such as statement execution and function
   * calls.
   *
   * WARNING: This interface is still a work in progress and is not yet
   * officially publicly available. It is likely to change in binary incompatible
   * (and possibly source incompatible) ways in future versions. It is
   * anticipated that at some stage the interface will be frozen and made
   * available for general use.
   */
  class KJS_EXPORT Debugger {
  public:

    /**
     * Creates a new debugger
     */
    Debugger();

    /**
     * Destroys the debugger. If the debugger is attached to any interpreters,
     * it is automatically detached.
     */
    virtual ~Debugger();

    DebuggerImp *imp() const { return rep; }

    /**
     * Attaches the debugger to specified interpreter. This will cause this
     * object to receive notification of events from the interpreter.
     *
     * If the interpreter is deleted, the debugger will automatically be
     * detached.
     *
     * Note: only one debugger can be attached to an interpreter at a time.
     * Attaching another debugger to the same interpreter will cause the
     * original debugger to be detached from that interpreter.
     *
     * @param interp The interpreter to attach to
     *
     * @see detach()
     */
    virtual void attach(Interpreter *interp);

    /**
     * Detach the debugger from an interpreter
     *
     * @param interp The interpreter to detach from. If 0, the debugger will be
     * detached from all interpreters to which it is attached.
     *
     * @see attach()
     */
    virtual void detach(Interpreter *interp);

    /**
     * Called to notify the debugger that some javascript source code has
     * been parsed. For calls to Interpreter::evaluate(), this will be called
     * with the supplied source code before any other code is parsed.
     * Other situations in which this may be called include creation of a
     * function using the Function() constructor, or the eval() function.
     *
     * The default implementation does nothing. Override this method if
     * you want to process this event.
     *
     * @param exec The current execution state
     * @param sourceId The ID of the source code (corresponds to the
     * sourceId supplied in other functions such as atStatement()
     * @param sourceURL Where the source code that was parsed came from
     * @param source The source code that was parsed
     * @param startingLineNumber The line number at which parsing started
     * @param errorLine The line number at which parsing encountered an
     * error, or -1 if the source code was valid and parsed successfully
     * @param errorMsg The error description, or null if the source code
       was valid and parsed successfully
     * @return true if execution should be continue, false if it should
     * be aborted
     */
    virtual bool sourceParsed(ExecState *exec, int sourceId, const UString &sourceURL,
                              const UString &source, int startingLineNumber, int errorLine, const UString &errorMsg);

    /**
     * Called when an exception is thrown during script execution.
     *
     * The default implementation does nothing. Override this method if
     * you want to process this event.
     *
     * @param exec The current execution state
     * @param sourceId The ID of the source code being executed
     * @param lineno The line at which the error occurred
     * @param exception The exception object
     * @return true if execution should be continue, false if it should
     * be aborted
     */
    virtual bool exception(ExecState *exec, int sourceId, int lineno,
                           JSValue *exception);

    bool hasHandledException(ExecState *, JSValue *);

    /**
     * Called when a line of the script is reached (before it is executed)
     *
     * The default implementation does nothing. Override this method if
     * you want to process this event.
     *
     * @param exec The current execution state
     * @param sourceId The ID of the source code being executed
     * @param firstLine The starting line of the statement  that is about to be
     * executed
     * @param lastLine The ending line of the statement  that is about to be
     * executed (usually the same as firstLine)
     * @return true if execution should be continue, false if it should
     * be aborted
     */
    virtual bool atStatement(ExecState *exec, int sourceId, int firstLine,
                             int lastLine);
    /**
     * Called when the interpreter enters a new execution context (stack
     * frame). This can happen in three situations:
     *
     * <ul>
     *   <li>A call to Interpreter::evaluate(). This has a codeType of
     *   GlobalCode </li>
     *   <li>A call to the builtin eval() function. The sourceId corresponds to
     *   the code passed in to eval. This has a codeType of EvalCode. The
     *   lineno here is always 0 since execution starts at the beginning of
     *   the script.</li>
     *   <li>A function call. This only occurs for functions defined in
     *   ECMAScript code, whether via the normal function() { ... } syntax or
     *   a call to the built-in Function() constructor (anonymous functions).
     *   In the former case, the sourceId and lineno indicate the location at
     *   which the function was defined. For anonymous functions, the sourceId
     *   corresponds to the code passed into the Function() constructor.</li>
     * </ul>
     *
     * enterContext() is not called for functions implemented in the native
     * code, since these do not use an execution context.
     *
     * @param exec The current execution state (corresponding to the new stack)
     * @param sourceId The ID of the source code being executed
     * @param lineno The line that is about to be executed
     * @param function The function being called. 0 in non-function context.
     * @param args The arguments that were passed to the function
     * line is being executed. Empty in non-function contexts.
     *
     * @return true if execution should be continued, false if it should
     * be aborted
     */
    virtual bool enterContext(ExecState *exec, int sourceId, int lineno,
                           JSObject *function, const List &args);

    /**
     * Called when the inteprreter exits an execution context. This always
     * corresponds to a previous call to enterContext()
     *
     * The default implementation does nothing. Override this method if
     * you want to process this event.
     *
     * @param exec The current execution state
     * @param sourceId The ID of the source code being executed
     * @param lineno The line that is about to be executed
     * @param function The function being returned from, if there is one
     * @return true if execution should be continue, false if it should
     * be aborted
     */
    virtual bool exitContext(ExecState *exec, int sourceId, int lineno,
                             JSObject *function);


    // Override this and return true if you want the debugger to report
    // pretty-printed versions of the source.
    virtual bool shouldReindentSources() const;

    // Override this to return true if the debugger should report
    // exceptions even if there is a try block waiting for it.
    virtual bool shouldReportCaught()    const;

    // The two methods below call the events but also keep track/use of line # information
    // so we can associate it with exceptions
    void reportAtStatement(ExecState *exec, int sourceId, int firstLine, int lastLine);
    void reportException  (ExecState *exec, JSValue *exception);

    // This notifies the debugger of source being parsed, reindenting it if need be.
    void reportSourceParsed(ExecState *exec, FunctionBodyNode *body, int sourceId, UString sourceURL, 
                              const UString &source, int startingLineNumber, int errorLine, const UString &errorMsg);
  private:
    DebuggerImp *rep;
    HashMap<Interpreter*, ProtectedPtr<JSValue> > latestExceptions;
    int lastLineRan;
    int lastSourceParsed; // Needed for attributing syntax exceptions at top-level
  public:
    static int debuggersPresent;
  };

}

#endif

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
