/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003, 2004, 2005, 2006 Apple Computer, Inc.
 *  Copyright (C) 2007, 2008 Maksim Orlovich (maksim@kde.org)
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
#include "CompileState.h"
#include "nodes.h"

#include <wtf/Assertions.h>
#include <wtf/Vector.h>

namespace KJS
{

CompileState::~CompileState()
{
    deleteAllValues(locals);
    deleteAllValues(freeMarkTemps);
    deleteAllValues(freeNonMarkTemps);
}

CodeBlock &CompileState::codeBlock()
{
    return fbody->code();
}

void CompileState::requestTemporary(OpType type, OpValue *value, OpValue *reference)
{
    ASSERT(type == OpType_value || type == OpType_bool || type == OpType_int32 || type == OpType_number);

    value->type      = type;
    value->immediate = false;

    reference->type      = OpType_reg;
    reference->immediate = true;

    RegDescriptor *temp = nullptr;

    bool markable = (type == OpType_value);

    if (markable && !freeMarkTemps.isEmpty()) {
        temp = freeMarkTemps.last();
        freeMarkTemps.removeLast();
    } else if (!markable && !freeNonMarkTemps.isEmpty()) {
        temp = freeNonMarkTemps.last();
        freeNonMarkTemps.removeLast();
    }

    if (!temp) {
        Register id = maxTemp;
        fbody->reserveSlot(id, markable);
        temp = new RegDescriptor(this, id, markable);
        ++maxTemp;
    }

    value->ownedReg = temp;

    reference->ownedReg = temp;
    reference->value.narrow.regVal = temp->reg();
}

OpValue CompileState::localReadVal(Register regNum)
{
    OpValue val;
    val.immediate = false;
    val.type      = OpType_value;

    RegDescriptor *desc = locals[regNum];
    if (!desc) {
        desc = new RegDescriptor(this, regNum, true, false /*not a temp!*/);
        locals[regNum] = desc;
    }

    val.ownedReg = desc;
    return val;
}

void CompileState::localFlushAll(CodeBlock &block)
{
    for (Register r = 0; r < initialMaxTemp; ++r) {
        if (locals[r] && locals[r]->live()) {
            flushLocal(block, r);
        }
    }
}

void CompileState::flushLocal(CodeBlock & /*block*/, Register regNum)
{
    if (locals[regNum] && locals[regNum]->live()) {
        OpValue localVal;
        localVal.immediate = false;
        localVal.type      = OpType_value;
        localVal.ownedReg  = locals[regNum];

        OpValue out, outReg;
        requestTemporary(OpType_value, &out, &outReg);

        CodeGen::emitOp(this, Op_RegPutValue, nullptr, &outReg, &localVal);

        // Now, patch up the descriptor to point to the same place as the temporary, and to
        // take ownership of it, and remove it from local descriptors list.
        locals[regNum]->adopt(out.ownedReg.get());
        locals[regNum] = nullptr;
    }
}

OpValue CompileState::localWriteRef(CodeBlock &block, Register regNum)
{
    // Detach any live value copies.
    flushLocal(block, regNum);

    OpValue rval;
    rval.immediate = true;
    rval.type      = OpType_reg;
    rval.value.narrow.regVal = regNum;
    return rval;
}

bool CompileState::pushLabel(const Identifier &label)
{
    if (!seenLabels.add(label).second) {
        return false;    // Dupe!
    }

    seenLabelsStack.append(label);
    pendingLabels.append(label);

    return true;
}

void CompileState::popLabel()
{
    Identifier name = seenLabelsStack.last();

    seenLabelsStack.removeLast();
    seenLabels.remove(name);
    labelTargets.remove(name);
    ASSERT(pendingLabels.isEmpty());
}

void CompileState::bindLabels(Node *node)
{
    for (size_t l = 0; l < pendingLabels.size(); ++l) {
        labelTargets.set(pendingLabels[l], node);
    }
    pendingLabels.clear();
}

Node *CompileState::resolveBreakLabel(Identifier label)
{
    if (label.isEmpty()) {
        if (defaultBreakTargets.isEmpty()) {
            return nullptr;
        } else {
            return defaultBreakTargets.last();
        }
    } else {
        return labelTargets.get(label);
    }
}

Node *CompileState::resolveContinueLabel(Identifier label)
{
    if (label.isEmpty()) {
        if (defaultContinueTargets.isEmpty()) {
            return nullptr;
        } else {
            return defaultContinueTargets.last();
        }
    } else {
        return labelTargets.get(label);
    }
}

void CompileState::pushNest(NestType type, Node *node)
{
    if (type == Scope) {
        ++scopeDepth;
    } else if (type == TryFinally) {
        ++finallyDepth;
    }

    NestInfo inf;
    inf.type = type;
    inf.node = node;
    nests.append(inf);

    assert(!(type == ContBreakTarget && !node));
}

void CompileState::popNest()
{
    if (nests.last().type == Scope) {
        --scopeDepth;
    } else if (nests.last().type == TryFinally) {
        --finallyDepth;
    }

    nests.removeLast();
}

void CompileState::pushDefaultBreak(Node *node)
{
    defaultBreakTargets.append(node);
}

void CompileState::pushDefaultContinue(Node *node)
{
    defaultContinueTargets.append(node);
}

void CompileState::popDefaultBreak()
{
    defaultBreakTargets.removeLast();
}

void CompileState::popDefaultContinue()
{
    defaultContinueTargets.removeLast();
}

void CompileState::addPendingBreak(Node *node, Addr addr)
{
    if (!pendingBreaks.contains(node)) {
        pendingBreaks.set(node, new WTF::Vector<Addr>());
    }
    pendingBreaks.get(node)->append(addr);
}

void CompileState::addPendingContinue(Node *node, Addr addr)
{
    if (!pendingContinues.contains(node)) {
        pendingContinues.set(node, new WTF::Vector<Addr>());
    }
    pendingContinues.get(node)->append(addr);
}

void CompileState::resolvePendingBreaks(Node *node, Addr dest)
{
    const WTF::Vector<Addr> *stats = pendingBreaks.get(node);
    if (!stats) {
        return;
    }

    CodeBlock &block = codeBlock();
    OpValue newDest = OpValue::immAddr(dest);
    for (size_t c = 0; c < stats->size(); ++c) {
        CodeGen::patchOpArgument(block, (*stats)[c], 0, newDest);
    }

    pendingBreaks.remove(node);
    delete stats;
}

void CompileState::resolvePendingContinues(Node *node, Addr dest)
{
    const WTF::Vector<Addr> *stats = pendingContinues.get(node);
    if (!stats) {
        return;
    }

    CodeBlock &block = codeBlock();
    OpValue newDest = OpValue::immAddr(dest);
    for (size_t c = 0; c < stats->size(); ++c) {
        CodeGen::patchOpArgument(block, (*stats)[c], 0, newDest);
    }

    pendingContinues.remove(node);
    delete stats;
}

static OpValue *addrDummy;

OpValue *OpValue::dummyAddr()
{
    if (!addrDummy) {
        addrDummy = new OpValue;
        *addrDummy = OpValue::immAddr(0);
    }
    return addrDummy;
}

} //namespace KJS

