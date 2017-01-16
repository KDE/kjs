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

#include "debugger.h"
#include "nodes.h"
#include "ustring.h"

#include "internal.h"

using namespace KJS;

// ------------------------------ Debugger -------------------------------------

namespace KJS
{
struct AttachedInterpreter {
    AttachedInterpreter(Interpreter *i, AttachedInterpreter *ai) : interp(i), next(ai)
    {
        ++Debugger::debuggersPresent;
    }
    ~AttachedInterpreter()
    {
        --Debugger::debuggersPresent;
    }
    Interpreter *interp;
    AttachedInterpreter *next;
};

}

int Debugger::debuggersPresent = 0;

Debugger::Debugger()
{
    lastLineRan = 0;
    rep = new DebuggerImp();
    lastSourceParsed = -1;
}

Debugger::~Debugger()
{
    detach(nullptr);
    delete rep;
}

void Debugger::attach(Interpreter *interp)
{
    Debugger *other = interp->debugger();
    if (other == this) {
        return;
    }
    if (other) {
        other->detach(interp);
    }
    interp->setDebugger(this);
    rep->interps = new AttachedInterpreter(interp, rep->interps);
}

void Debugger::detach(Interpreter *interp)
{
    // iterate the addresses where AttachedInterpreter pointers are stored
    // so we can unlink items from the list
    AttachedInterpreter **p = &rep->interps;
    AttachedInterpreter *q;
    while ((q = *p)) {
        if (!interp || q->interp == interp) {
            *p = q->next;
            q->interp->setDebugger(nullptr);
            delete q;
        } else {
            p = &q->next;
        }
    }

    if (interp) {
        latestExceptions.remove(interp);
    } else {
        latestExceptions.clear();
    }
}

bool Debugger::hasHandledException(ExecState *exec, JSValue *exception)
{
    if (latestExceptions.get(exec->dynamicInterpreter()).get() == exception) {
        return true;
    }

    latestExceptions.set(exec->dynamicInterpreter(), exception);
    return false;
}

bool Debugger::sourceParsed(ExecState * /*exec*/, int /*sourceId*/, const UString &/*sourceURL*/,
                            const UString &/*source*/, int /*startingLineNumber*/, int /*errorLine*/, const UString & /*errorMsg*/)
{
    return true;
}

bool Debugger::exception(ExecState * /*exec*/, int /*sourceId*/, int /*lineno*/,
                         JSValue * /*exception*/)
{
    return true;
}

bool Debugger::atStatement(ExecState * /*exec*/, int /*sourceId*/, int /*firstLine*/,
                           int /*lastLine*/)
{
    return true;
}

void Debugger::reportAtStatement(ExecState *exec, int sourceId, int firstLine, int lastLine)
{
    lastLineRan = firstLine;
    atStatement(exec, sourceId, firstLine, lastLine);
}

void Debugger::reportException(ExecState *exec, JSValue *exceptionVal)
{
    if (!hasHandledException(exec, exceptionVal)) {
        exception(exec, exec->currentBody() ? exec->currentBody()->sourceId() : lastSourceParsed, lastLineRan, exceptionVal);
    }
}

bool Debugger::enterContext(ExecState * /*exec*/, int /*sourceId*/, int /*lineno*/,
                            JSObject * /*function*/, const List & /*args*/)
{
    return true;
}

bool Debugger::exitContext(ExecState * /*exec*/, int /*sourceId*/, int /*lineno*/,
                           JSObject * /*function*/)
{
    return true;
}

bool Debugger::shouldReindentSources() const
{
    return false;
}

bool Debugger::shouldReportCaught() const
{
    return false;
}

void Debugger::reportSourceParsed(ExecState *exec, FunctionBodyNode *body,
                                  int sourceId, UString sourceURL, const UString &source,
                                  int startingLineNumber, int errorLine, const UString &errorMsg)
{
    lastSourceParsed = sourceId;
    UString code = source;
    if (shouldReindentSources() && body) {
        code = body->reindent(startingLineNumber);
    }
    sourceParsed(exec, sourceId, sourceURL, code, startingLineNumber, errorLine, errorMsg);
}
