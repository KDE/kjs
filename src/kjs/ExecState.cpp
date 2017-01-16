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

#include "ExecState.h"
#include "function.h"
#include "scriptfunction.h"
#include "internal.h"
#include "nodes.h"
#include "debugger.h"

namespace KJS
{

Interpreter *ExecState::lexicalInterpreter() const
{
    JSObject *outerScope = scopeChain().bottom();
    assert(outerScope->isGlobalObject());

    Interpreter *result = static_cast<JSGlobalObject *>(outerScope)->interpreter();

    if (!result) {
        return dynamicInterpreter();
    }

    return result;
}

void ExecState::markSelf()
{
    if (m_codeType != FunctionCode && m_localStore) {
        //### some code dupe here with JSVariableObject::mark. Not sure how to best
        // restructure.

        // Note: the m_localStore check is needed here, since for non-function code,
        // we may create function object in declaration elaboration stage, before
        // compilation and set up of this
        size_t size                = m_localStoreSize;
        LocalStorageEntry *entries = m_localStore;

        for (size_t i = 0; i < size; ++i) {
            JSValue *value = entries[i].val.valueVal;
            if (!(entries[i].attributes & DontMark) && !value->marked()) {
                value->mark();
            }
        }
    }

    for (size_t i = 0; i < m_deferredCompletions.size(); ++i) {
        JSValue *e = m_deferredCompletions[i].value();
        if (e && !e->marked()) {
            e->mark();
        }
    }

    JSValue *e = m_completion.value();
    if (e && !e->marked()) {
        e->mark();
    }

    scope.mark();

    // Propagate up to other eval chains..
    if (m_savedExec && m_savedExec != m_callingExec) {
        ASSERT(m_savedExec != this);
        m_savedExec->mark();
    }
}

void ExecState::mark()
{
    for (ExecState *exec = this; exec; exec = exec->m_callingExec) {
        exec->markSelf();
    }
}

ExecState::ExecState(Interpreter *intp, ExecState *save) :
    m_interpreter(intp),
    m_propertyNames(CommonIdentifiers::shared()),
    m_callingExec(nullptr),
    m_savedExec(save),
    m_currentBody(nullptr),
    m_function(nullptr),
    m_localStore(nullptr),
    m_pcBase(nullptr),
    m_pc(nullptr),
    m_machineLocalStore(nullptr)
{
    /**
     The reason we need m_savedExec and can't just be content with m_callingExec is two-fold.
     First of all, in many cases KHTML (and ktranscript) invoke functions such as event handlers
     on globalExec. When that happens, we still need to be able to mark the previous call-chain.
     Also, it is possible for the client to call Interpreter::evaluate again; and we still
     need to mark things from the outside when that happens
    */

    if (m_callingExec && m_savedExec && m_callingExec != m_savedExec) {
        assert(m_callingExec == intp->globalExec());
    }
    m_interpreter->setExecState(this);
}

ExecState::~ExecState()
{
    m_interpreter->setExecState(m_savedExec);
}

void ExecState::pushExceptionHandler(HandlerType type, Addr addr)
{
    m_exceptionHandlers.append(ExceptionHandler(type, addr));
}

void ExecState::popExceptionHandler()
{
    m_exceptionHandlers.removeLast();
}

JSValue *ExecState::reactivateCompletion(bool insideTryFinally)
{
    // First, unwind and get the old completion..
    ASSERT(m_exceptionHandlers.last().type == RemoveDeferred);
    popExceptionHandler();
    Completion comp = m_deferredCompletions.last();
    m_deferredCompletions.removeLast();

    // Now, our behavior behaves on whether we're inside an another
    // try..finally or not. If we're, we must route even
    // continue/break/return completions via the EH machinery;
    // if not, we execute them directly
    if (comp.complType() == Normal) {
        // We just straight fell into 'finally'. Nothing fancy to do.
        return nullptr;
    }

    if (comp.complType() == Throw || insideTryFinally) {
        setAbruptCompletion(comp);
    } else {
        if (comp.complType() == ReturnValue) {
            return comp.value();
        } else {
            assert(comp.complType() == Break || comp.complType() == Continue);
            *m_pc = m_pcBase + comp.target();
        }
    }

    return nullptr;
}

void ExecState::setException(JSValue *e)
{
    if (e) {
        setAbruptCompletion(Completion(Throw, e));
    } else {
        clearException();
    }
}

void ExecState::setAbruptCompletion(Completion comp)
{
    // If we already had an exception, merely update the object, to permit
    // users to refine the exception, being careful not to double-unwind.
    // However, warn about it in debug builds.
    if (hadException()) {
#ifndef NDEBUG
        printInfo(this, "warning: overriding already set exception ", m_completion.value());
        printInfo(this, "with ", comp.value());
#endif

        m_completion = comp;
        return;
    }

    // Trace to debugger if needed.
    Debugger *dbg = dynamicInterpreter()->debugger();
    if (dbg && comp.complType() == Throw) {
        dbg->reportException(this, comp.value());
    }

    m_completion = comp;

    while (!m_exceptionHandlers.isEmpty()) {
        switch (m_exceptionHandlers.last().type) {
        case JumpToCatch:
            *m_pc = m_pcBase + m_exceptionHandlers.last().dest;
            m_exceptionHandlers.removeLast();
            return; // done handling it
        case PopScope:
            popScope();
            m_exceptionHandlers.removeLast();
            continue; // get the next handler
        case RemoveDeferred:
            m_deferredCompletions.removeLast();
            m_exceptionHandlers.removeLast();
            continue; // get the next handler
        case Silent:
            // Exception blocked by tracing code. nothing to do.
            return;
        }
    }
}

void ExecState::quietUnwind(int depth)
{
    ASSERT(m_exceptionHandlers.size() >= size_t(depth));
    for (int e = 0; e < depth; ++e) {
        HandlerType type = m_exceptionHandlers.last().type;
        m_exceptionHandlers.removeLast();

        switch (type) {
        case JumpToCatch:
            break; //Nothing to do here!
        case PopScope:
            popScope();
            break;
        case RemoveDeferred:
            m_deferredCompletions.removeLast();
            break;
        case Silent:
            ASSERT(0); // Should not happen in the middle of the code.
            break;
        }
    }
}

GlobalExecState::GlobalExecState(Interpreter *intp, JSGlobalObject *glob): ExecState(intp, nullptr /* nothing else constructed yet*/)
{
    scope.push(glob);
    m_codeType  = GlobalCode;
    m_variable = glob;
    m_thisVal  = glob;
}

InterpreterExecState::InterpreterExecState(Interpreter *intp, JSGlobalObject *glob,
        JSObject *thisObject, ProgramNode *body):
    ExecState(intp, intp->execState())
{
    m_currentBody = body;
    scope.push(glob);
    m_codeType = GlobalCode;
    m_variable = glob;
    // Per 10.2.1, we should use the global object here, but
    // Interpreter::evaluate permits it to be overridden, e.g. for LiveConnect.
    m_thisVal  = thisObject;
}

EvalExecState::EvalExecState(Interpreter *intp, JSGlobalObject *glob,
                             ProgramNode *body, ExecState *callingExecState):
    ExecState(intp, intp->execState())
{
    m_currentBody = body;
    m_codeType    = EvalCode;
    m_callingExec = callingExecState;
    if (m_callingExec) {
        scope = m_callingExec->scopeChain();
        m_variable = m_callingExec->variableObject();
        m_thisVal  = m_callingExec->thisValue();
        return;
    }

    // 10.2.2 talks about the behavior w/o a calling context here,
    // saying it should be like global code. This can not happen
    // in actual JS code, but it may be synthesized by e.g.
    // the JS debugger calling 'eval' itself, from globalExec
    m_thisVal  = glob;
    m_variable = glob;
    scope.push(glob);
}

FunctionExecState::FunctionExecState(Interpreter *intp, JSObject *thisObject,
                                     FunctionBodyNode *body, ExecState *callingExecState,
                                     FunctionImp *function): ExecState(intp, intp->execState())
{
    m_function    = function;
    m_currentBody = body;

    m_codeType    = FunctionCode;
    m_callingExec = callingExecState;
    scope = function->scope(); // Activation will push itself when setting up
    m_variable = m_interpreter->getRecycledActivation();// TODO: DontDelete ? (ECMA 10.2.3)
    if (!m_variable) {
        m_variable = new ActivationImp();
    }
    m_thisVal  = thisObject;
}

} // namespace KJS

