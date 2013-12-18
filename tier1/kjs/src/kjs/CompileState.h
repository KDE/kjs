// -*- c-basic-offset: 2 -*-
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
#ifndef COMPILE_STATE_H
#define COMPILE_STATE_H

#include "ExecState.h" // For codetype... Kinda odd.

#include "opcodes.h"
#include "bytecode/opargs.h"

#include <wtf/Assertions.h>
#include <wtf/HashSet.h>
#include <wtf/HashMap.h>

using WTF::HashSet;
using WTF::HashMap;
using WTF::Vector;


namespace KJS {

class RegDescriptor;
class FunctionBodyNode;


enum CompileType
{
    NotCompiled,
    Release,
    Debug
};

class CompileState
{
public:
    CompileState(CodeType ctype, CompileType compType, FunctionBodyNode* fbody, Register initialMaxTemp):
        localScopeVal(0), thisVal(0), globalScopeVal(0), evalResRegister(0),
        ctype(ctype), compType(compType), locals(initialMaxTemp, 0), initialMaxTemp(initialMaxTemp),
        maxTemp(initialMaxTemp), fbody(fbody), scopeDepth(0), finallyDepth(0), neededClosures(false)
    { }

    FunctionBodyNode* functionBody() {
        return fbody;
    }

    CodeType codeType() const {
        return ctype;
    }

    CodeBlock& codeBlock();

    CompileType compileType() const {
        return compType;
    }


    ~CompileState();

    // Returns true if the register is a formal temporary.
    bool isTemporaryReg(Register regNum) {
        return regNum >= initialMaxTemp;
    }

    // We distinguish two kinds of temporaries --- markable and not. They'll get
    // corresponding bits set in localStore when that's initialized.
    void requestTemporary(OpType type, OpValue* value, OpValue* reference);

    // This method is used to acquire a read value of a local...
    OpValue localReadVal(Register regNum);

    // And this one returns a reference, acquiring it for (immediate) write.
    // If there are any active read copies, we will backup the old value to
    // a temporary, and petchup their register descriptor to point to the backup.
    OpValue localWriteRef(CodeBlock& block, Register regNum);

    // This forces all live locals to temporaries.
    void localFlushAll(CodeBlock& block);

    // This sets the registers containing the local scope and
    // 'this' values... It should be the rvalue, not the regnums
    void setPreloadRegs(OpValue* localReg, OpValue* globalReg, OpValue* thisReg) {
        localScopeVal  = localReg;
        globalScopeVal = globalReg;
        thisVal        = thisReg;
    }

    OpValue* localScope() {
        return localScopeVal;
    }

    OpValue* thisValue() {
        return thisVal;
    }

    OpValue* globalScope() {
        return globalScopeVal;
    }

    void setEvalResultRegister(OpValue* val) {
        evalResRegister = val;
    }

    OpValue* evalResultReg() {
        return evalResRegister;
    }

    // To properly implement operations like continue and break, we need to keep track whether we
    // are nested inside with, try-catch and try-finally operations.
    // This serves two purposes:
    // 1) if we're not jumping out of a try-finally, we have to unwind the cleanup stacks
    // 2) if we're inside a try-finally, we have to jump to the finally and not
    //    do the normal operation (this applies to return as well)
    // Also, if we're inside a 'with' or a catch we cannot optimize local variable access.

    enum NestType {
        Scope,
        OtherCleanup,
        TryFinally,
        ContBreakTarget
    };

    void pushNest(NestType type, Node* node = 0);
    void popNest ();

    struct NestInfo {
        NestType type;
        Node*    node;
    };

    bool inNestedScope() {
        return scopeDepth > 0;
    }

    bool inTryFinally() {
        return finallyDepth > 0;
    }

    const WTF::Vector<NestInfo>&  nestStack() {
        return nests;
    }

    // Some constructs can be detected at compile time to involve
    // taking of closures. We keep track of that and avoid stack-allocation
    // if those are present.
    bool needsClosures() {
        return neededClosures;
    }

    void setNeedsClosures() {
        neededClosures = true;
    }

    // Label stuff....
    
    // Registers a pending label. Returns true if the label is OK, false if it's a duplicate.
    // If it fails, the label stack isn't touched!
    bool pushLabel(const Identifier& label);
    void popLabel();

    // Binds all the labels to the given node
    void bindLabels(Node* node);

    // Returns destination for the label (node will be 0 if not found)
    Node* resolveContinueLabel(Identifier label);
    Node* resolveBreakLabel   (Identifier label);

    // Sets the targets for break/continues w/o label name
    void pushDefaultBreak   (Node* node);
    void pushDefaultContinue(Node* node);
    void popDefaultBreak   ();
    void popDefaultContinue();

    // Helpers for these and resolvePendingBreak
    void enterLoop(Node* node) {
        pushNest(ContBreakTarget, node);
        pushDefaultBreak(node);
        pushDefaultContinue(node);
    }

    void exitLoop(Node* node) {
        popNest();
        popDefaultBreak();
        popDefaultContinue();
        resolvePendingBreaks(node, CodeGen::nextPC(this));
    }

    // Adds break/continue as needing relevant target for given node
    void addPendingBreak   (Node* node, Addr addr);
    void addPendingContinue(Node* node, Addr addr);

    // Patches up all pending break/continue statements to given destination.
    // LabelNode takes care of the breaks itself, the loops need to deal
    // with continue, though.
    void resolvePendingBreaks   (Node* node, Addr dest);
    void resolvePendingContinues(Node* node, Addr dest);
private:
    OpValue* localScopeVal;
    OpValue* thisVal;
    OpValue* globalScopeVal;
    OpValue* evalResRegister;

    CodeType ctype;
    CompileType compType;

    // Makes sure that any values of a local are 
    void flushLocal(CodeBlock& block, Register reg);

    friend class RegDescriptor;
    WTF::Vector<RegDescriptor*> locals;
    WTF::Vector<RegDescriptor*> freeMarkTemps;
    WTF::Vector<RegDescriptor*> freeNonMarkTemps;
    Register initialMaxTemp;
    Register maxTemp;

    FunctionBodyNode* fbody;

    void reuse(RegDescriptor* desc, bool markable) {
        if (markable)
            freeMarkTemps.append(desc);
        else
            freeNonMarkTemps.append(desc);
    }

    // Cached version of #of Scopes's from below.
    int scopeDepth;

    // Cached version of #of Finally's from below...
    int finallyDepth;
    
    WTF::Vector<NestInfo> nests;

    // This is true if we see code constructs that require taking a closure
    // inside here, which means we should not stack-allocate activations.
    bool neededClosures;

    // Label resolution..
    WTF::HashSet<Identifier> seenLabels;    // all labels we're inside
    WTF::Vector <Identifier> seenLabelsStack;
    WTF::Vector <Identifier> pendingLabels; // labels tha that haven't been bound to
                                            // a statement yet.

    // Targets for continue/break w/o destination.
    WTF::Vector<Node*> defaultBreakTargets;
    WTF::Vector<Node*> defaultContinueTargets;

    // Named label targets
    WTF::HashMap<Identifier, Node*> labelTargets;

    WTF::HashMap<Node*, WTF::Vector<Addr>* > pendingBreaks;
    WTF::HashMap<Node*, WTF::Vector<Addr>* > pendingContinues;
};

// We used register descriptors for two reasons:
// 1) For temporaries, we ref-counted them by OpValue in order to manage their lifetime
// 2) For locals, we use them to do COW of values...
class RegDescriptor
{
public:
    RegDescriptor(CompileState* owner, Register reg, bool markable, bool temp = true):
        owner(owner), regNo(reg), temp(temp), markable(markable), killed(false), refCount(0)
    {}

    Register reg() const {
        return regNo;
    }

    void ref() {
        ++refCount;
    }

    void deref() {
        --refCount;
        if (refCount == 0) {
            if (killed)
                delete this;
            else if (temp)
                owner->reuse(this, markable);
        }
    }

    bool live() {
        return refCount > 0;
    }

    void adopt(RegDescriptor* other) {
        // Make this point to the same as an another descriptor, which is about to die..
        temp     = other->temp;
        markable = other->markable;
        regNo    = other->regNo;

        // Mark the other descriptor as killed, as we took ownership of this.
        other->killed = true;
    }
private:
    CompileState* owner;
    Register      regNo;
    bool temp;
    bool markable;
    bool killed;
    int  refCount;
};

inline OpValue OpValue::immInt32(int32_t in) {
    OpValue res;
    initImm(&res, OpType_int32);
    res.value.narrow.int32Val = in;
    return res;
}

inline OpValue OpValue::immNumber(double in) {
    OpValue res;
    initImm(&res, OpType_number);
    res.value.wide.numberVal = in;
    return res;
}

inline OpValue OpValue::immValue(JSValue* in) {
    assert(in);
    OpValue res;
    initImm(&res, OpType_value);
    res.value.wide.valueVal = in;
    return res;
}

inline OpValue OpValue::immBool(bool in) {
    OpValue res;
    initImm(&res, OpType_bool);
    res.value.narrow.boolVal = in;
    return res;
}

inline OpValue OpValue::immString(UString* in) {
    OpValue res;
    initImm(&res, OpType_string);
    res.value.wide.stringVal = in;
    return res;
}

inline OpValue OpValue::immIdent(Identifier* in) {
    OpValue res;
    initImm(&res, OpType_ident);
    res.value.wide.identVal = in;
    return res;
}

inline OpValue OpValue::immNode(KJS::Node* in) {
    OpValue res;
    initImm(&res, OpType_node);
    res.value.wide.nodeVal = in;
    return res;
}

inline OpValue OpValue::immCStr(const char* in) {
    OpValue res;
    initImm(&res, OpType_cstr);
    res.value.wide.cstrVal = in;
    return res;
}

inline OpValue OpValue::immAddr(Addr in) {
    OpValue res;
    initImm(&res, OpType_addr);
    res.value.narrow.addrVal = in;
    return res;
}

inline OpValue::OpValue(): type(OpType_void) {} // since should never occur as an argument..

}

#endif
// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
