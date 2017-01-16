/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003, 2007, 2008 Apple Inc. All rights reserved.
 *  Copyright (C) 2008 Maksim Orlovich (maksim@kde.org)
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

#ifndef ExecState_H
#define ExecState_H

#include "completion.h"
#include "value.h"
#include "types.h"
#include "CommonIdentifiers.h"
#include "scope_chain.h"
#include "LocalStorage.h"
#include "wtf/Vector.h"
#include "PropertyNameArray.h"

namespace KJS
{
class ActivationImp;
class Interpreter;
class FunctionImp;
class FunctionBodyNode;
class ProgramNode;
class JSGlobalObject;

enum CodeType { GlobalCode, EvalCode, FunctionCode };

/**
 * Represents the current state of script execution. This object allows you
 * obtain a handle the interpreter that is currently executing the script,
 * and also the current execution context.
 */
class KJS_EXPORT ExecState : Noncopyable
{
    friend class Interpreter;
    friend class FunctionImp;
    friend class GlobalFuncImp;
public:
    /**
     * Returns the interpreter associated with this execution state
     *
     * @return The interpreter executing the script
     */
    Interpreter *dynamicInterpreter() const
    {
        return m_interpreter;
    }

    /**
     * Returns the interpreter associated with the current scope's
     * global object
     *
     * @return The interpreter currently in scope
     */
    Interpreter *lexicalInterpreter() const;

    /**
     * This describes how an exception should be handled
     */
    enum HandlerType {
        JumpToCatch,     ///< jump to the specified address
        PopScope,        ///< remove a scope chain entry, and run the next handler
        RemoveDeferred,  ///< remove any deferred exception object, and run the next entry
        Silent           ///< just update the exception object. For debugger-type use only
    };

    void pushExceptionHandler(HandlerType type, Addr addr = 0);

    void popExceptionHandler();

    // Cleanup depth entries from the stack, w/o running jumps
    void quietUnwind(int depth);

    void setMachineRegisters(const unsigned char *pcBase, const unsigned char **pcLoc, LocalStorageEntry **machineLocalStoreLoc)
    {
        m_pcBase            = pcBase;
        m_pc                = pcLoc;
        m_machineLocalStore = machineLocalStoreLoc;
    }

    /**
     The below methods deal with deferring of completions inside finally clauses.
     Essentially, we clear any set exceptions and memorize any non-normal completion
     (including the target addresses for the continue/break statements) on
     the m_deferredCompletions stack. If the 'finally' finishes normally,
     we will resume the previous completion. If not, finally's abnormal
     termination is handled as usually; a RemoveDeferred cleanup stack
     entry is added to unwind m_deferredCompletions if that happens.
    */

    void deferCompletion()
    {
        pushExceptionHandler(RemoveDeferred);
        m_deferredCompletions.append(abruptCompletion());
        clearException();
    }

    /**
     This resumes dispatch of a completion that was deferred due to a try ... finally,
     handling it as appropriate for whether it's inside an another try-finally.
     This will handle all the cases itself except for one: return,
     for which it will return the value to return (otherwise returning 0)
    */
    JSValue *reactivateCompletion(bool insideTryFinally);

    /**
     * Set the exception associated with this execution state,
     * updating the program counter appropriately, and executing any relevant EH cleanups.
     * @param e The JSValue of the exception being set
     */
    void setException(JSValue *e);

    /**
     * Records an abrupt completion of code, and jumps to the closest catch or finally.
     * This always happens for exceptions, but can also happen for continue/break/return when
     * they're inside try ... finally, since that case gets routed through the EH machinery.
     */
    void setAbruptCompletion(Completion comp);

    /**
     * Clears the exception or other abnormal completion set on this execution state.
     */
    void clearException()
    {
        m_completion = Completion();
    }

    /**
     * Returns the exception associated with this execution state.
     * @return The current execution state exception
     */
    JSValue *exception() const
    {
        return m_completion.complType() == Throw ? m_completion.value() : nullptr;
    }

    /**
     * Use this to check if an exception was thrown in the current
     * execution state.
     *
     * @return Whether an exception was thrown
     */
    bool hadException() const
    {
        return m_completion.complType() == Throw;
    }

    Completion abruptCompletion() const
    {
        return m_completion;
    }

    /**
     * Returns the scope chain for this execution context. This is used for
     * variable lookup, with the list being searched from start to end until a
     * variable is found.
     *
     * @return The execution context's scope chain
     */
    const ScopeChain &scopeChain() const
    {
        return scope;
    }

    /**
     * Returns the variable object for the execution context. This contains a
     * property for each variable declared in the execution context.
     *
     * @return The execution context's variable object
     */
    JSObject *variableObject() const
    {
        return m_variable;
    }
    void setVariableObject(JSObject *v)
    {
        m_variable = v;
    }

    /**
     * Returns the "this" value for the execution context. This is the value
     * returned when a script references the special variable "this". It should
     * always be an Object, unless application-specific code has passed in a
     * different type.
     *
     * The object that is used as the "this" value depends on the type of
     * execution context - for global contexts, the global object is used. For
     * function objewcts, the value is given by the caller (e.g. in the case of
     * obj.func(), obj would be the "this" value). For code executed by the
     * built-in "eval" function, the this value is the same as the calling
     * context.
     *
     * @return The execution context's "this" value
     */
    JSObject *thisValue() const
    {
        return m_thisVal;
    }

    /**
     * Returns the context from which the current context was invoked. For
     * global code this will be a null context (i.e. one for which
     * isNull() returns true). You should check isNull() on the returned
     * value before calling any of its methods.
     *
     * @return The calling execution context
     */
    ExecState *callingExecState()
    {
        return m_callingExec;
    }

    /**
     * Returns the execState of a previous nested evaluation session, if any.
     */
    ExecState *savedExecState()
    {
        return m_savedExec;
    }

    JSObject *activationObject()
    {
        assert(m_codeType == FunctionCode);
        return m_variable;
    }

    CodeType codeType()
    {
        return m_codeType;
    }
    FunctionBodyNode *currentBody()
    {
        return m_currentBody;
    }
    FunctionImp *function() const
    {
        return m_function;
    }

    void pushVariableObjectScope(JSVariableObject *s)
    {
        scope.pushVariableObject(s);
    }
    void pushScope(JSObject *s)
    {
        scope.push(s);
    }
    void popScope()
    {
        scope.pop();
    }

    void mark();

    void initLocalStorage(LocalStorageEntry *store, size_t size)
    {
        m_localStore = store;
        m_localStoreSize = size;
    }

    void updateLocalStorage(LocalStorageEntry *newStore)
    {
        m_localStore         = newStore;
        *m_machineLocalStore = newStore;
    }

    LocalStorageEntry *localStorage()
    {
        return m_localStore;
    }

    // This is a workaround to avoid accessing the global variables for these identifiers in
    // important property lookup functions, to avoid taking PIC branches in Mach-O binaries
    const CommonIdentifiers &propertyNames() const
    {
        return *m_propertyNames;
    }

    // Compatibility stuff:
    ExecState *context()
    {
        return this;
    }
    ExecState *callingContext()
    {
        return callingExecState();
    }
protected:
    ExecState(Interpreter *intp, ExecState *save);
    ~ExecState();
    void markSelf();

    Interpreter *m_interpreter;
    Completion   m_completion;
    CommonIdentifiers *m_propertyNames;
    ExecState *m_callingExec;
    ExecState *m_savedExec; // in case of recursion of evaluation. Needed to mark things properly;
    // note that this is disjoint from the above, since that's only used for
    // eval/function, while this is for global.

    FunctionBodyNode *m_currentBody;
    FunctionImp *m_function;

    ScopeChain scope;
    JSObject *m_variable;
    JSObject *m_thisVal;

    LocalStorageEntry      *m_localStore;
    size_t                  m_localStoreSize;

    struct ExceptionHandler {
        ExceptionHandler() {}
        ExceptionHandler(HandlerType type, Addr dest):
            type(type), dest(dest) {}

        HandlerType type;
        Addr        dest;
    };

    const unsigned char  *m_pcBase;  // The address of pc = 0
    const unsigned char **m_pc;      // Where the current fetch address is stored
    LocalStorageEntry **m_machineLocalStore; // Machine's copy of m_localStore
    WTF::Vector<ExceptionHandler, 4> m_exceptionHandlers;
    WTF::Vector<Completion, 4>       m_deferredCompletions;

    CodeType m_codeType;
};

typedef ExecState Context; // Compatibility only

class GlobalExecState : public ExecState
{
public:
    GlobalExecState(Interpreter *intp, JSGlobalObject *global);
};

class InterpreterExecState : public ExecState
{
public:
    InterpreterExecState(Interpreter *intp, JSGlobalObject *global, JSObject *thisObject, ProgramNode *);
};

class EvalExecState : public ExecState
{
public:
    EvalExecState(Interpreter *intp, JSGlobalObject *global, ProgramNode *body, ExecState *callingExecState);
};

// Note: this does not push the activation on the scope chain,
// as the activation is not initialized at this point.
class FunctionExecState : public ExecState
{
public:
    FunctionExecState(Interpreter *intp, JSObject *thisObject,
                      FunctionBodyNode *, ExecState *callingExecState, FunctionImp *);
};

} // namespace KJS

#endif // ExecState_H
