/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2002 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003, 2004, 2005, 2006, 2007 Apple Inc. All rights reserved.
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
 #include "nodes2bytecode.h"
 #include "CompileState.h"
 #include <wtf/Assertions.h>

 #include <typeinfo>
 #include <iostream>

namespace KJS {

// A few helpers..
static void emitError(CompileState* comp, Node* node, ErrorType type, const char* msgStr)
{
    OpValue me = OpValue::immNode(node);
    OpValue se = OpValue::immInt32(type);
    OpValue msg = OpValue::immCStr(msgStr);
    CodeGen::emitOp(comp, Op_RaiseError, 0, &me, &se, &msg);
}

static void emitSyntaxError(CompileState* comp, Node* node, const char* msgStr)
{
    emitError(comp, node, SyntaxError, msgStr);
}

static void emitReferenceError(CompileState* comp, Node* node, const char* msgStr)
{
    emitError(comp, node, ReferenceError, msgStr);
}


OpValue Node::generateEvalCode(CompileState*)
{
    std::cerr << "WARNING: no generateEvalCode for:" << typeid(*this).name() << "\n";
    ASSERT(0);

    return OpValue::immInt32(42);
}

void StatementNode::generateExecCode(CompileState*)
{
    std::cerr << "WARNING: no generateExecCode for:" << typeid(*this).name() << "\n";
    ASSERT(0);
}

void StatementNode::generateDebugInfo(CompileState* comp)
{
    OpValue me = OpValue::immNode(this);
    CodeGen::emitOp(comp, Op_AtStatement, 0, &me);
}

static inline bool exitContextNeeded(CompileState* comp) {
    return comp->compileType() == Debug &&
           comp->codeType()    == FunctionCode;
}

static void generateExitContextIfNeeded(CompileState* comp) {
    if (exitContextNeeded(comp)) {
        OpValue ourNode = OpValue::immNode(comp->functionBody());
        CodeGen::emitOp(comp, Op_ExitDebugContext, 0, &ourNode);        
    }
}

// ------------------------------ Basic literals -----------------------------------------

OpValue NullNode::generateEvalCode(CompileState*)
{
    return OpValue::immValue(jsNull());
}

OpValue BooleanNode::generateEvalCode(CompileState*)
{
    return OpValue::immBool(value());
}

OpValue NumberNode::generateEvalCode(CompileState*)
{
    using namespace std;
#if 0
    if (typeHint == OpType_Value) {
        // Try to fit into a JSValue if at all possible..
        JSValue* im = JSImmediate::from(value());
        if (im) {
            OpValue res = mkImmediateVal(OpType_value);
            return res;
        }
    }
#endif

    // Numeric-like..
    double d = value();
    int32_t i32 = JSValue::toInt32(d);
    if (double(i32) == d && !(i32 == 0 && signbit(d))) // be careful with -0.0 here
        return OpValue::immInt32(i32);
    else
        return OpValue::immNumber(d);
}


OpValue StringNode::generateEvalCode(CompileState* comp)
{
    // For now, just generate a JSValue
    // We may want to permit string pointers as well, to help overload resolution,
    // but it's not clear whether that's useful, since we can't MM them. Perhaps
    // a special StringInstance type may be of use eventually.
    
    if (interned) // we're re-compiling.. just reuse it
        return OpValue::immValue(interned);
        
    // Intern shorter strings
    if (val.size() < 16) {
        interned = Interpreter::internString(val);
        return OpValue::immValue(interned);
    } else {
        OpValue inStr = OpValue::immString(&val);

        OpValue out;
        CodeGen::emitOp(comp, Op_OwnedString, &out, &inStr);
        return out;
    }
}

StringNode::~StringNode()
{
    if (interned)
        Interpreter::releaseInternedString(val);
}

OpValue RegExpNode::generateEvalCode(CompileState* comp)
{
    // ### TODO: cache the engine object?
    OpValue out;
    OpValue patternV = OpValue::immString(&pattern);
    OpValue flagsV   = OpValue::immString(&flags);
    CodeGen::emitOp(comp, Op_NewRegExp, &out, &patternV, &flagsV);
    return out;
}

OpValue ThisNode::generateEvalCode(CompileState* comp)
{
    return *comp->thisValue();
}

// ------------------------------ VarAccessNode ----------------------------------------

size_t VarAccessNode::classifyVariable(CompileState* comp, Classification& classify)
{
    // Are we inside a with or catch? In that case, it's all dynamic. Boo.
    // Ditto for eval.
    // ### actually that may be improvable if we can
    // distinguish eval-from-global and eval-from-local, since
    // we'd have an activation or global object available for access.
    if (comp->inNestedScope() || comp->codeType() == EvalCode) {
        classify = Dynamic;
        return missingSymbolMarker();
    }

    // If we're inside global scope (and as per above, not inside any nested scope!)
    // we can always used the global object
    if (comp->codeType() == GlobalCode) {
        classify = Global;
        return missingSymbolMarker();
    }

    // We're inside a function...
    if (ident == CommonIdentifiers::shared()->arguments) {
        // arguments is too much of a pain to handle in general path..
        classify = Dynamic;
        return missingSymbolMarker();
    }

    // Do we know this?
    size_t index = comp->functionBody()->lookupSymbolID(ident);
    if (index == missingSymbolMarker())
        classify = NonLocal;
    else
        classify = Local;

    return index;
}

OpValue VarAccessNode::generateEvalCode(CompileState* comp)
{
    Classification classify;
    size_t index = classifyVariable(comp, classify);

    OpValue out;
    OpValue varName = OpValue::immIdent(&ident);
    switch (classify) {
    case Local: {
        // Register read.
        out = comp->localReadVal(index);
        break;
    }
    case NonLocal:
        CodeGen::emitOp(comp, Op_NonLocalVarGet, &out, &varName);
        break;
    case Global:
        CodeGen::emitOp(comp, Op_GlobalObjectGet, &out, &varName);
        break;
    case Dynamic:
        CodeGen::emitOp(comp, Op_VarGet, &out, &varName);
        break;
    }

    return out;
}

OpValue VarAccessNode::valueForTypeOf(CompileState* comp)
{
    // ### some code dupe here.
    Classification classify;
    size_t index = classifyVariable(comp, classify);

    OpValue scopeTemp;
    OpValue out, outReg;
    OpValue varName = OpValue::immIdent(&ident);
    switch (classify) {
    case Local:
        // Register read. Easy.
        out = comp->localReadVal(index);
        break;
    case Global:
        CodeGen::emitOp(comp, Op_SymGetKnownObject, &out, comp->globalScope(), &varName);
        break;
    case NonLocal:
        comp->requestTemporary(OpType_value, &out, &outReg);
        CodeGen::emitOp(comp, Op_NonLocalScopeLookupAndGet, &scopeTemp, &outReg, &varName);
        break;
    case Dynamic:
        comp->requestTemporary(OpType_value, &out, &outReg);
        CodeGen::emitOp(comp, Op_ScopeLookupAndGet, &scopeTemp, &outReg, &varName);
        break;
    }

    return out;
}

CompileReference* VarAccessNode::generateRefBind(CompileState* comp)
{
    Classification classify;
    classifyVariable(comp, classify);

    if (classify == Local || classify == Global)
        return 0; // nothing to do, we know where it is

    // Otherwise, we need to find the scope for writing
    CompileReference* ref = new CompileReference;

    OpValue quiet = OpValue::immNode(0);
    OpValue varName = OpValue::immIdent(&ident);
    CodeGen::emitOp(comp, classify == Dynamic ? Op_ScopeLookup : Op_NonLocalScopeLookup,
                        &ref->baseObj, &varName, &quiet);
    return ref;
}

CompileReference* VarAccessNode::generateRefRead(CompileState* comp, OpValue* out)
{
    Classification classify;
    classifyVariable(comp, classify);

    // We want to bind and read, also issuing an error.

    // If we don't need any binding, just use normal read code..
    if (classify == Local || classify == Global) {
        *out = generateEvalCode(comp);
        return 0;
    }

    // For others, use the lookup-and-fetch ops
    CompileReference* ref = new CompileReference;

    OpValue readReg;
    OpValue varName = OpValue::immIdent(&ident);
    comp->requestTemporary(OpType_value, out, &readReg);

    OpName op;
    if (classify == Dynamic)
        op = Op_ScopeLookupAndGetChecked;
    else
        op = Op_NonLocalScopeLookupAndGetChecked;
    CodeGen::emitOp(comp, op, &ref->baseObj, &readReg, &varName);

    return ref;
}

void VarAccessNode::generateRefWrite(CompileState* comp,
                                           CompileReference* ref, OpValue& valToStore)
{
    Classification classify;
    size_t index = classifyVariable(comp, classify);

    if (classify == Local) {
        // Straight register put..
        OpValue destReg = comp->localWriteRef(comp->codeBlock(), index);
        CodeGen::emitOp(comp, Op_RegPutValue, 0, &destReg, &valToStore);
    } else {
        // Symbolic write to the appropriate scope..
        OpValue varName = OpValue::immIdent(&ident);
        CodeGen::emitOp(comp, Op_SymPutKnownObject, 0,
                       (classify == Global ? comp->globalScope() : &ref->baseObj), &varName, &valToStore);
    }
}

OpValue VarAccessNode::generateRefDelete(CompileState* comp)
{
    Classification classify;
    classifyVariable(comp, classify);

    if (classify == Local) {
        // Normal locals are DontDelete, so this always fails.
        return OpValue::immBool(false);
    }

    // Otherwise, fetch the appropriate scope
    OpValue base;
    if (classify == Global) {
        base = *comp->globalScope();
    } else {
        OpValue varName = OpValue::immIdent(&ident);
        OpValue silent  = OpValue::immNode(0);
        CodeGen::emitOp(comp, classify == Dynamic ? Op_ScopeLookup : Op_NonLocalScopeLookup,
                        &base, &varName, &silent);
    }

    // Remove the property..
    OpValue out;
    OpValue varName = OpValue::immIdent(&ident);
    CodeGen::emitOp(comp, Op_SymDeleteKnownObject, &out, &base, &varName);
    return out;
}

void VarAccessNode::generateRefFunc(CompileState* comp, OpValue* funOut, OpValue* thisOut)
{
    Classification classify;
    classifyVariable(comp, classify);

    OpValue varName = OpValue::immIdent(&ident);

    OpValue thisReg;
    switch (classify) {
    case Local:
    case Global:
        // Both of these use global object for this, and use straightforward lookup for value
        *funOut  = generateEvalCode(comp);
        *thisOut = *comp->globalScope();
        break;
    case NonLocal:
        comp->requestTemporary(OpType_value, thisOut, &thisReg);
        CodeGen::emitOp(comp, Op_NonLocalFunctionLookupAndGet, funOut, &thisReg, &varName);
        break;
    case Dynamic:
        comp->requestTemporary(OpType_value, thisOut, &thisReg);
        CodeGen::emitOp(comp, Op_FunctionLookupAndGet, funOut, &thisReg, &varName);
        break;
    }
}

// ------------------------------ GroupNode----------------------------------------

OpValue GroupNode::generateEvalCode(CompileState* comp)
{
    return group->generateEvalCode(comp);
}

// ------------------------------ Object + Array literals --------------------------

OpValue ArrayNode::generateEvalCode(CompileState* comp)
{
    OpValue arr;
    CodeGen::emitOp(comp, Op_NewEmptyArray, &arr);

    OpValue und = OpValue::immValue(jsUndefined());

    int pos = 0;
    for (ElementNode* el = element.get(); el; el = el->next.get()) {
        if (!el->node) {
            // Fill elision w/undefined, unless we can just skip over to a value
            for (int i = 0; i < el->elision; i++) {
                OpValue ind = OpValue::immInt32(pos);
                CodeGen::emitOp(comp, Op_BracketPutKnownObject, 0, &arr, &ind, &und);
                ++pos;
            }
        } else {
            pos += el->elision;
        }

        if (el->node) {
            OpValue val = el->node->generateEvalCode(comp);
            OpValue ind = OpValue::immInt32(pos);
            CodeGen::emitOp(comp, Op_BracketPutKnownObject, 0, &arr, &ind, &val);
            ++pos;
        }
    }

    for (int i = 0; i < elision; i++) {
        OpValue ind = OpValue::immInt32(pos);
        CodeGen::emitOp(comp, Op_BracketPutKnownObject, 0, &arr, &ind, &und);
        ++pos;
    }

    return arr;
}

OpValue ObjectLiteralNode::generateEvalCode(CompileState* comp)
{
    OpValue obj;
    CodeGen::emitOp(comp, Op_NewObject, &obj);

    for (PropertyListNode* entry = list.get(); entry; entry = entry->next.get()) {
        PropertyNode* prop = entry->node.get();
        OpValue name = OpValue::immIdent(&prop->name->str);
        OpValue val  = prop->assign->generateEvalCode(comp);

        switch (prop->type) {
        case PropertyNode::Getter:
            CodeGen::emitOp(comp, Op_DefineGetter, 0, &obj, &name, &val);
            break;
        case PropertyNode::Setter:
            CodeGen::emitOp(comp, Op_DefineSetter, 0, &obj, &name, &val);
            break;
        case PropertyNode::Constant:
            CodeGen::emitOp(comp, Op_SymPutKnownObject, 0, &obj, &name, &val);
            break;
        }
    }

    return obj;
}

// ------------------------------ BracketAccessorNode --------------------------------
OpValue BracketAccessorNode::generateEvalCode(CompileState* comp)
{
    OpValue ret;
    OpValue base  = expr1->generateEvalCode(comp);
    OpValue index = expr2->generateEvalCode(comp);

    // ### optimize foo["bar"] ?
    CodeGen::emitOp(comp, Op_BracketGet, &ret, &base, &index);
    return ret;
}

CompileReference* BracketAccessorNode::generateRefBind(CompileState* comp)
{
    // Per 11.2.1, the following steps must happen when evaluating foo[bar]
    // 1) eval foo
    // 2) eval bar
    // 3) call toObject on [[foo]]
    // 4) call toString on [[bar]]
    // ... all of which are part of reference evaluation. Fun.
    // ### FIXME FIXME FIXME: we don't do step 4 in right spot yet!
    CompileReference* ref = new CompileReference;
    OpValue baseV = expr1->generateEvalCode(comp);
    ref->indexVal = expr2->generateEvalCode(comp);
    CodeGen::emitOp(comp, Op_ToObject, &ref->baseObj, &baseV);
    return ref;
}

CompileReference* BracketAccessorNode::generateRefRead(CompileState* comp, OpValue* out)
{
    CompileReference* ref = new CompileReference;

    // ### As above, this sequence should store the toString on reference, if there will be a follow up
    // write --- need a hint for that..
    OpValue baseV = expr1->generateEvalCode(comp);
    ref->indexVal = expr2->generateEvalCode(comp);

    // Store the object for future use.
    OpValue baseReg;
    comp->requestTemporary(OpType_value, &ref->baseObj, &baseReg);

    CodeGen::emitOp(comp, Op_BracketGetAndBind, out, &baseReg, &baseV, &ref->indexVal);
    return ref;
}

void BracketAccessorNode::generateRefWrite(CompileState* comp,
                                             CompileReference* ref, OpValue& valToStore)
{
    CodeGen::emitOp(comp, Op_BracketPutKnownObject, 0, &ref->baseObj, &ref->indexVal, &valToStore);
}

OpValue BracketAccessorNode::generateRefDelete(CompileState* comp)
{
    OpValue base  = expr1->generateEvalCode(comp);
    OpValue index = expr2->generateEvalCode(comp);

    OpValue out;
    CodeGen::emitOp(comp, Op_BracketDelete, &out, &base, &index);
    return out;
}

void BracketAccessorNode::generateRefFunc(CompileState* comp, OpValue* funOut, OpValue* thisOut)
{
    OpValue baseV  = expr1->generateEvalCode(comp);
    OpValue indexV = expr2->generateEvalCode(comp);

    // We need to memorize the toObject for 'this'
    OpValue baseReg;
    comp->requestTemporary(OpType_value, thisOut, &baseReg);

    CodeGen::emitOp(comp, Op_BracketGetAndBind, funOut, &baseReg, &baseV, &indexV);
}

// ------------------------------ DotAccessorNode --------------------------------

// ECMA 11.2.1b
OpValue DotAccessorNode::generateEvalCode(CompileState* comp)
{
    OpValue ret;
    OpValue base    = expr->generateEvalCode(comp);
    OpValue varName = OpValue::immIdent(&ident);
    CodeGen::emitOp(comp, Op_SymGet, &ret, &base, &varName);
    return ret;
}

CompileReference* DotAccessorNode::generateRefBind(CompileState* comp)
{
    CompileReference* ref = new CompileReference;
    OpValue baseV = expr->generateEvalCode(comp);
    CodeGen::emitOp(comp, Op_ToObject, &ref->baseObj, &baseV);
    return ref;
}

CompileReference* DotAccessorNode::generateRefRead(CompileState* comp, OpValue* out)
{
    CompileReference* ref = new CompileReference;
    OpValue baseV = expr->generateEvalCode(comp);
    OpValue baseReg;
    OpValue varName = OpValue::immIdent(&ident);
    comp->requestTemporary(OpType_value, &ref->baseObj, &baseReg);
    CodeGen::emitOp(comp, Op_SymGetAndBind, out, &baseReg, &baseV, &varName);
    return ref;
}

void DotAccessorNode::generateRefWrite(CompileState* comp,
                                       CompileReference* ref, OpValue& valToStore)
{
    OpValue varName = OpValue::immIdent(&ident);
    CodeGen::emitOp(comp, Op_SymPutKnownObject, 0, &ref->baseObj, &varName, &valToStore);
}

OpValue DotAccessorNode::generateRefDelete(CompileState* comp)
{
    OpValue base = expr->generateEvalCode(comp);
    OpValue varName = OpValue::immIdent(&ident);
    OpValue out;
    CodeGen::emitOp(comp, Op_SymDelete, &out, &base, &varName);
    return out;
}

void DotAccessorNode::generateRefFunc(CompileState* comp, OpValue* funOut, OpValue* thisOut)
{
    OpValue baseV = expr->generateEvalCode(comp);
    OpValue varName = OpValue::immIdent(&ident);

    OpValue baseReg;
    comp->requestTemporary(OpType_value, thisOut, &baseReg);
    CodeGen::emitOp(comp, Op_SymGetAndBind, funOut, &baseReg, &baseV, &varName);
}

// ------------------ ........

void ArgumentsNode::generateEvalArguments(CompileState* comp)
{
    WTF::Vector<OpValue> args;

    // We need evaluate arguments and push them in separate steps as there may be
    // function/ctor calls inside.
    for (ArgumentListNode* arg = list.get(); arg; arg = arg->next.get()) {
        args.append(arg->expr->generateEvalCode(comp));
    }

    CodeGen::emitOp(comp, Op_ClearArgs, 0);

    size_t c = 0;
    while (c < args.size()) {
        if (c + 3 <= args.size()) {
            CodeGen::emitOp(comp, Op_Add3Arg, 0, &args[c], &args[c+1], &args[c+2]);
            c += 3;
        } else if (c + 2 <= args.size()) {
            CodeGen::emitOp(comp, Op_Add2Arg, 0, &args[c], &args[c+1]);
            c += 2;
        } else {
            CodeGen::emitOp(comp, Op_AddArg, 0, &args[c]);
            c += 1;
        }
    }
}

OpValue NewExprNode::generateEvalCode(CompileState* comp)
{
    OpValue v = expr->generateEvalCode(comp);

    if (args)
        args->generateEvalArguments(comp);
    else
        CodeGen::emitOp(comp, Op_ClearArgs, 0);

    OpValue out;
    CodeGen::emitOp(comp, Op_CtorCall, &out, &v);
    return out;
}

OpValue FunctionCallValueNode::generateEvalCode(CompileState* comp)
{
    OpValue v = expr->generateEvalCode(comp);
    args->generateEvalArguments(comp);

    OpValue out;
    CodeGen::emitOp(comp, Op_FunctionCall, &out, &v, comp->globalScope());
    return out;
}

OpValue FunctionCallReferenceNode::generateEvalCode(CompileState* comp)
{
    Node* cand = expr->nodeInsideAllParens();
    ASSERT(cand->isLocation());
    LocationNode* loc = static_cast<LocationNode*>(cand);

    OpValue funVal, thisVal;
    loc->generateRefFunc(comp, &funVal, &thisVal);
    args->generateEvalArguments(comp);

    OpValue out;
    CodeGen::emitOp(comp, Op_FunctionCall, &out, &funVal, &thisVal);
    return out;
}

OpValue PostfixNode::generateEvalCode(CompileState* comp)
{
    Node* cand = m_loc->nodeInsideAllParens();
    if (!cand->isLocation()) {
        emitReferenceError(comp, this,
                m_oper == OpPlusPlus ?
                    "Postfix ++ operator applied to value that is not a reference." :
                    "Postfix -- operator applied to value that is not a reference.");
        return OpValue::immValue(jsUndefined());
    }

    LocationNode* loc = static_cast<LocationNode*>(cand);

    // ### we want to fold this in if the kid is a local -- any elegant way?

    //read current value
    OpValue curV;
    CompileReference* ref = loc->generateRefRead(comp, &curV);

    // We need it to be a number..
    if (curV.type != OpType_number) {
        OpValue numVal;
        CodeGen::emitConvertTo(comp, &curV, OpType_number, &numVal);
        curV = numVal;
    }

    // Compute new one
    OpValue newV;
    CodeGen::emitOp(comp, (m_oper == OpPlusPlus) ? Op_Add1 : Op_Sub1,
                    &newV, &curV);

    loc->generateRefWrite(comp, ref, newV);
    delete ref;
    return curV;
}

OpValue DeleteReferenceNode::generateEvalCode(CompileState* comp)
{
    return loc->generateRefDelete(comp);
}

OpValue DeleteValueNode::generateEvalCode(CompileState*)
{
    return OpValue::immBool(true);
}

OpValue VoidNode::generateEvalCode(CompileState* comp)
{
    (void)expr->generateEvalCode(comp);
    return OpValue::immValue(jsUndefined());
}

OpValue TypeOfVarNode::generateEvalCode(CompileState* comp)
{
    OpValue v = loc->valueForTypeOf(comp);

    OpValue out;
    CodeGen::emitOp(comp, Op_TypeOf, &out, &v);
    return out;
}

OpValue TypeOfValueNode::generateEvalCode(CompileState* comp)
{
    OpValue v = m_expr->generateEvalCode(comp);
    OpValue typeOfV;
    CodeGen::emitOp(comp, Op_TypeOf, &typeOfV, &v);
    return typeOfV;
}

OpValue PrefixNode::generateEvalCode(CompileState* comp)
{
    Node* cand = m_loc->nodeInsideAllParens();
    if (!cand->isLocation()) {
        emitReferenceError(comp, this,
                m_oper == OpPlusPlus ?
                    "Prefix ++ operator applied to value that is not a reference." :
                    "Prefix -- operator applied to value that is not a reference.");
        return OpValue::immValue(jsUndefined());
    }

    LocationNode* loc = static_cast<LocationNode*>(cand);

    // ### we want to fold this in if the kid is a local -- any elegant way?

    //read current value
    OpValue curV;
    CompileReference* ref = loc->generateRefRead(comp, &curV);

    OpValue newV;
    CodeGen::emitOp(comp, (m_oper == OpPlusPlus) ? Op_Add1 : Op_Sub1,
                    &newV, &curV);

    // Write out + return new value.
    loc->generateRefWrite(comp, ref, newV);
    delete ref;
    return newV;
}

OpValue UnaryPlusNode::generateEvalCode(CompileState* comp)
{
    // This is basically just a number cast
    OpValue curV = expr->generateEvalCode(comp);

    if (curV.type != OpType_number) {
        OpValue numVal;
        CodeGen::emitConvertTo(comp, &curV, OpType_number, &numVal);
        curV = numVal;
    }

    return curV;
}

OpValue NegateNode::generateEvalCode(CompileState* comp)
{
    OpValue v = expr->generateEvalCode(comp);
    OpValue negV;
    CodeGen::emitOp(comp, Op_Neg, &negV, &v);
    return negV;
}

OpValue BitwiseNotNode::generateEvalCode(CompileState* comp)
{
    OpValue v = expr->generateEvalCode(comp);
    OpValue out;
    CodeGen::emitOp(comp, Op_BitNot, &out, &v);
    return out;
}

OpValue LogicalNotNode::generateEvalCode(CompileState* comp)
{
    OpValue v = expr->generateEvalCode(comp);
    OpValue out;
    CodeGen::emitOp(comp, Op_LogicalNot, &out, &v);
    return out;
}

OpValue BinaryOperatorNode::generateEvalCode(CompileState* comp)
{
    OpValue v1 = expr1->generateEvalCode(comp);
    OpValue v2 = expr2->generateEvalCode(comp);

    OpName codeOp; // ### could perhaps skip conversion entirely,
                   // and set these in the parser?
    switch (oper) {
    case OpMult:
        // operator *
        codeOp = Op_Mult;
        break;
    case OpDiv:
        // operator /
        codeOp = Op_Div;
        break;
    case OpMod:
        // operator %
        codeOp = Op_Mod;
        break;
    case OpPlus:
        // operator +
        codeOp = Op_Add;
        break;
    case OpMinus:
        // operator -
        codeOp = Op_Sub;
        break;
    case OpLShift:
        // operator <<
        codeOp = Op_LShift;
        break;
    case OpRShift:
        // operator >>
        codeOp = Op_RShift;
        break;
    case OpURShift:
        // operator >>>
        codeOp = Op_URShift;
        break;
    case OpLess:
        // operator <
        codeOp = Op_Less;
        break;
    case OpGreaterEq:
        // operator >=
        codeOp = Op_GreaterEq;
        break;
    case OpGreater:
        // operator >
        codeOp = Op_Greater;
        break;
    case OpLessEq:
        // operator <=
        codeOp = Op_LessEq;
        break;
    case OpEqEq:
        // operator ==
        codeOp = Op_EqEq;
        break;
    case OpNotEq:
        // operator !=
        codeOp = Op_NotEq;
        break;
    case OpStrEq:
        // operator ===
        codeOp = Op_StrEq;
        break;
    case OpStrNEq:
        // operator !==
        codeOp = Op_StrNEq;
        break;
    case OpBitAnd:
        // operator &
        codeOp = Op_BitAnd;
        break;
    case OpBitXOr:
        // operator ^
        codeOp = Op_BitXOr;
        break;
    case OpBitOr:
        // operator |
        codeOp = Op_BitOr;
        break;
    case OpIn:
        codeOp = Op_In;
        break;
    case OpInstanceOf:
        codeOp = Op_InstanceOf;
        break;

    default:
        assert(!"BinaryOperatorNode: unhandled switch case");
    }

    OpValue out;
    CodeGen::emitOp(comp, codeOp, &out, &v1, &v2);
    return out;
}

OpValue BinaryLogicalNode::generateEvalCode(CompileState* comp)
{
    // This is somewhat ugly since we can't patchup labels in already generated
    // code, and don't know the types in advance. It could also benefit from
    // a type hint, since it's easier if we only want a bool, which is quite common

    OpValue a = expr1->generateEvalCode(comp);

    // Make a register for storing the result, and put 'a' there, as out first guess.
    OpValue aVal, aReg;
    comp->requestTemporary(a.type, &aVal, &aReg);
    CodeGen::emitRegStore(comp, &aReg, &a);

    // Is this enough to shortcircuit?
    // if op is && and a is false, we jump out, ditto
    // for || and true.
    Addr jumpToShortCircuit = CodeGen::emitOp(comp, oper == OpAnd ? Op_IfNotJump : Op_IfJump,
                                              0, &a, OpValue::dummyAddr());

    // Now, generate the code for b...
    OpValue b = expr2->generateEvalCode(comp);

    // Hopefully, either the types match, or the result slot is already a value,
    // so we can just promote b (which will happen automatically to produce param for Op_RegPutVal)
    if (a.type == b.type || a.type == OpType_value) {
        if (a.type == OpType_value)
            CodeGen::emitOp(comp, Op_RegPutValue, 0, &aReg, &b);
        else
            CodeGen::emitRegStore(comp, &aReg, &b);
        CodeGen::patchJumpToNext(comp, jumpToShortCircuit, 1);
        return aVal;
    } else {
        // We need to promote 'a' as well, which means we need to skip over the code jumpToShortCircuit
        // went to after handling store of 'b'.

        // Get a new register for the result, put b there..
        OpValue resVal, resReg;
        comp->requestTemporary(OpType_value, &resVal, &resReg);
        CodeGen::emitOp(comp, Op_RegPutValue, 0, &resReg, &b);

        // skip to after a promotion..
        Addr jumpToAfter = CodeGen::emitOp(comp, Op_Jump, 0, OpValue::dummyAddr());

        // a's promotion goes here..
        CodeGen::patchJumpToNext(comp, jumpToShortCircuit, 1);
        CodeGen::emitOp(comp, Op_RegPutValue, 0, &resReg, &a);

        // now we're after it..
        CodeGen::patchJumpToNext(comp, jumpToAfter, 0);

        return resVal;
    }
}

OpValue ConditionalNode::generateEvalCode(CompileState* comp)
{
    // As above, we have some difficulty here, since we do not have a way of knowing
    // the types in advance, but since we can't reasonably speculate on them both being bool,
    // we just always produce a value.
    OpValue resVal, resReg;

    // Evaluate conditional, and jump..
    OpValue v = logical->generateEvalCode(comp);
    Addr jumpToElse = CodeGen::emitOp(comp, Op_IfNotJump, 0, &v, OpValue::dummyAddr());

    // True branch
    OpValue v1out = expr1->generateEvalCode(comp);

    // Request a temporary for the result. (We can't reuse any, since it may be a variable!)
    // ### perhaps do an isTemporary check here?
    comp->requestTemporary(OpType_value, &resVal, &resReg);
    CodeGen::emitOp(comp, Op_RegPutValue, 0, &resReg, &v1out);

    Addr jumpToAfter = CodeGen::emitOp(comp, Op_Jump, 0, OpValue::dummyAddr());

    // Jump to else goes here.
    CodeGen::patchJumpToNext(comp, jumpToElse, 1);

    // : part..
    OpValue v2out = expr2->generateEvalCode(comp);
    CodeGen::emitOp(comp, Op_RegPutValue, 0, &resReg, &v2out);

    // After everything
    CodeGen::patchJumpToNext(comp, jumpToAfter, 0);

    return resVal;
}

OpValue FuncExprNode::generateEvalCode(CompileState* comp)
{
    comp->setNeedsClosures();

    OpValue out;
    OpValue nameV = OpValue::immIdent(&ident);
    OpValue bodyV = OpValue::immNode(body.get());
    CodeGen::emitOp(comp, Op_EvalFuncExpr, &out, &nameV, &bodyV);
    return out;
}

void FuncDeclNode::generateExecCode(CompileState* comp)
{
    comp->setNeedsClosures();

    // No executable content...
}

void SourceElementsNode::generateExecCode(CompileState* comp)
{
    node->generateExecCode(comp);

    // ### FIXME: how do we do proper completion?
    for (SourceElementsNode *n = next.get(); n; n = n->next.get()) {
        n->node->generateExecCode(comp);
    }
}

OpValue AssignNode::generateEvalCode(CompileState* comp)
{
    Node* cand = m_loc->nodeInsideAllParens();
    if (!cand->isLocation()) {
        emitReferenceError(comp, this, "Left side of assignment is not a reference.");
        return OpValue::immValue(jsUndefined());
    }

    LocationNode* loc = static_cast<LocationNode*>(cand);

    CompileReference* ref;

    OpValue v;
    if (m_oper == OpEqual) {
        ref = loc->generateRefBind(comp);
        v = m_right->generateEvalCode(comp);
    } else {
        OpValue v1;
        ref = loc->generateRefRead(comp, &v1);
        OpValue v2 = m_right->generateEvalCode(comp);

        OpName codeOp;
        switch (m_oper) {
        case OpMultEq:
            codeOp = Op_Mult;
            break;
        case OpDivEq:
            codeOp = Op_Div;
            break;
        case OpModEq:
            codeOp = Op_Mod;
            break;
        case OpPlusEq:
            codeOp = Op_Add;
            break;
        case OpMinusEq:
            codeOp = Op_Sub;
            break;
        case OpLShift:
            codeOp = Op_LShift;
            break;
        case OpRShift:
            codeOp = Op_RShift;
            break;
        case OpURShift:
            codeOp = Op_URShift;
            break;
        case OpAndEq:
            codeOp = Op_BitAnd;
            break;
        case OpXOrEq:
            codeOp = Op_BitXOr;
            break;
        case OpOrEq:
            codeOp = Op_BitOr;
            break;
        default:
            ASSERT(0);
        }

        CodeGen::emitOp(comp, codeOp, &v, &v1, &v2);
    }

    loc->generateRefWrite(comp, ref, v);

    delete ref;
    return v;
}

OpValue CommaNode::generateEvalCode(CompileState* comp)
{
    expr1->generateEvalCode(comp);
    return expr2->generateEvalCode(comp);
}

OpValue AssignExprNode::generateEvalCode(CompileState* comp)
{
    return expr->generateEvalCode(comp);
}

void VarDeclNode::generateCode(CompileState* comp)
{
    // We only care about things which have an initializer ---
    // everything else is a no-op at execution time,
    // and only makes a difference at processVarDecl time
    if (init) {
        if (comp->inNestedScope()) {
            // We need to do the full lookup mess, which includes doing split binding and store
            OpValue quiet   = OpValue::immNode(0);
            OpValue varName = OpValue::immIdent(&ident);
            OpValue base;
            CodeGen::emitOp(comp, Op_ScopeLookup, &base, &varName, &quiet);

            OpValue val = init->generateEvalCode(comp);
            CodeGen::emitOp(comp, Op_SymPutKnownObject, 0, &base, &varName, &val);
            return;
        }

        OpValue val = init->generateEvalCode(comp);
        size_t localID = comp->functionBody()->lookupSymbolID(ident);
        if (localID == missingSymbolMarker()) {
            // Generate a symbolic assignment, always to local scope
            OpValue identV = OpValue::immIdent(&ident);
            CodeGen::emitOp(comp, Op_SymPutKnownObject, 0, comp->localScope(), &identV, &val);
        } else {
            // Store to the local..
            OpValue dest = comp->localWriteRef(comp->codeBlock(), localID);
            CodeGen::emitOp(comp, Op_RegPutValue, 0, &dest, &val);
        }
    } // if initializer..
}

OpValue VarDeclListNode::generateEvalCode(CompileState* comp)
{
    for (VarDeclListNode *n = this; n; n = n->next.get())
        n->var->generateCode(comp);

    return OpValue::immInt32(0); // unused..
}

void VarStatementNode::generateExecCode(CompileState* comp)
{
    generateDebugInfoIfNeeded(comp);
    next->generateEvalCode(comp);
}

void BlockNode::generateExecCode(CompileState* comp)
{
    if (source) {
        generateDebugInfoIfNeeded(comp);
        source->generateExecCode(comp);
    }
}

void EmptyStatementNode::generateExecCode(CompileState*)
{}

void ExprStatementNode::generateExecCode(CompileState* comp)
{
    generateDebugInfoIfNeeded(comp);
    OpValue val = expr->generateEvalCode(comp);

    // Update the result for eval or global code
    if (comp->codeType() != FunctionCode)
        CodeGen::emitOp(comp, Op_RegPutValue, 0, comp->evalResultReg(), &val);
}

void IfNode::generateExecCode(CompileState* comp)
{
    generateDebugInfoIfNeeded(comp);
    
    // eval the condition
    OpValue cond = expr->generateEvalCode(comp);

    // If condition is not true, jump to after or else..
    Addr afterTrueJmp = CodeGen::emitOp(comp, Op_IfNotJump, 0, &cond, OpValue::dummyAddr());

    // Emit the body of true...
    statement1->generateExecCode(comp);

    // If we have an else, add in a jump to skip over it.
    Addr afterAllJmp = 0;
    if (statement2)
        afterAllJmp = CodeGen::emitOp(comp, Op_Jump, 0, OpValue::dummyAddr());

    // This is where we go if true fails --- else, or afterwards.
    CodeGen::patchJumpToNext(comp, afterTrueJmp, 1);

    if (statement2) {
        // Body of else
        statement2->generateExecCode(comp);

        // Fix up the jump-over code
        CodeGen::patchJumpToNext(comp, afterAllJmp, 0);
    }
}

void DoWhileNode::generateExecCode(CompileState* comp)
{
    generateDebugInfoIfNeeded(comp);
    comp->enterLoop(this);

    // Body
    OpValue beforeBody = OpValue::immAddr(CodeGen::nextPC(comp));
    statement->generateExecCode(comp);

    // continues go to just before the test..
    comp->resolvePendingContinues(this, CodeGen::nextPC(comp));

    // test
    OpValue cond = expr->generateEvalCode(comp);
    CodeGen::emitOp(comp, Op_IfJump, 0, &cond, &beforeBody);

    comp->exitLoop(this);
}

void WhileNode::generateExecCode(CompileState* comp)
{
    generateDebugInfoIfNeeded(comp);
    comp->enterLoop(this);

    // Jump to test.
    Addr  jumpToTest = CodeGen::emitOp(comp, Op_Jump, 0, OpValue::dummyAddr());

    // Body
    OpValue beforeBody = OpValue::immAddr(CodeGen::nextPC(comp));
    statement->generateExecCode(comp);

    // continues go to just before the test..
    comp->resolvePendingContinues(this, CodeGen::nextPC(comp));

    // patch up the destination of the initial jump to test
    CodeGen::patchJumpToNext(comp, jumpToTest, 0);

    // test
    OpValue cond = expr->generateEvalCode(comp);
    CodeGen::emitOp(comp, Op_IfJump, 0, &cond, &beforeBody);

    comp->exitLoop(this);
}

void ForNode::generateExecCode(CompileState* comp)
{
    generateDebugInfoIfNeeded(comp);
    comp->enterLoop(this);

    // Initializer, if any..
    if (expr1)
        expr1->generateEvalCode(comp);

    // Insert a jump to the loop test (address not yet known)
    Addr jumpToTest = CodeGen::emitOp(comp, Op_Jump, 0, OpValue::dummyAddr());

    // Generate loop body..
    OpValue bodyAddr = OpValue::immAddr(CodeGen::nextPC(comp));
    statement->generateExecCode(comp);

    // We're about to generate the increment... The continues should go here..
    comp->resolvePendingContinues(this, CodeGen::nextPC(comp));

    // ### there is a CheckTimeout hook here in nodes.cpp...

    // Generate increment...
    if (expr3)
      expr3->generateEvalCode(comp);

    // The test goes here, so patch up the previous jump..
    CodeGen::patchJumpToNext(comp, jumpToTest, 0);

    // Make the test itself --- if it exists..
    if (expr2) {
        OpValue cond = expr2->generateEvalCode(comp);
        CodeGen::emitOp(comp, Op_IfJump, 0, &cond, &bodyAddr);
    } else {
        // Just jump back to the body.
        CodeGen::emitOp(comp, Op_Jump, 0, &bodyAddr);
    }

    comp->exitLoop(this);
}

void ForInNode::generateExecCode(CompileState* comp)
{
    generateDebugInfoIfNeeded(comp);
    if (varDecl)
        varDecl->generateCode(comp);

    OpValue val = expr->generateEvalCode(comp);
    OpValue obj; // version of val after toObject, returned by BeginForIn.
    
    OpValue stateVal, stateReg;
    comp->requestTemporary(OpType_value, &stateVal, &stateReg);

    // Fetch the property name array..
    CodeGen::emitOp(comp, Op_BeginForIn, &obj, &val, &stateReg);

    comp->enterLoop(this); 

    // We put the test first here, since the test and the fetch are combined.
    OpValue sym;
    Addr fetchNext = CodeGen::emitOp(comp, Op_NextForInEntry, &sym, &obj,
                                     &stateVal, OpValue::dummyAddr());

    // Write to the variable
    assert (lexpr->isLocation());
    LocationNode* loc = static_cast<LocationNode*>(lexpr.get());

    CompileReference* ref = loc->generateRefBind(comp);
    loc->generateRefWrite (comp, ref, sym);
    delete ref;

    // Run the body.
    statement->generateExecCode(comp);

    // Can fix the continues to go back to the test...
    comp->resolvePendingContinues(this, fetchNext);

    // Jump back..
    OpValue backVal = OpValue::immAddr(fetchNext);
    CodeGen::emitOp(comp, Op_Jump, 0, &backVal);

    // The end address is here (3 argument + return val)
    CodeGen::patchJumpToNext(comp, fetchNext, 3);

    comp->exitLoop(this);
}

// Helper for continue/break -- emits stack cleanup call if needed,
// and a jump either to the or an ??? exception.
static void handleJumpOut(CompileState* comp, Node* dest, ComplType breakOrCont)
{
    // We scan up the nest stack until we get to the target or
    // a try-finally.
    int toUnwind = 0;

    const WTF::Vector<CompileState::NestInfo>& nests = comp->nestStack();

    for (int pos = nests.size() - 1; pos >= 0; --pos) {
        switch (nests[pos].type) {
        case CompileState::Scope:
        case CompileState::OtherCleanup:
            ++toUnwind;
            break;
        case CompileState::TryFinally: {
            // Uh-oh. We have to handle this via exception machinery, giving it the
            // original address
            Addr    pc    = CodeGen::nextPC(comp);
            CodeGen::emitOp(comp, Op_ContBreakInTryFinally, 0, OpValue::dummyAddr());

            // Queue destination for resolution
            if (breakOrCont == Continue)
                comp->addPendingContinue(dest, pc);
            else
                comp->addPendingBreak(dest, pc);

            return;
        }

        case CompileState::ContBreakTarget:
            if (nests[pos].node == dest) {
                // Great. We found where we're going! Emit the unwind instr (if needed),
                // and the jump.
                if (toUnwind) {
                    OpValue unwind = OpValue::immInt32(toUnwind);
                    CodeGen::emitOp(comp, Op_UnwindStacks, 0, &unwind);
                }

                // Emit a jump...
                Addr    pc    = CodeGen::nextPC(comp);
                CodeGen::emitOp(comp, Op_Jump, 0, OpValue::dummyAddr());

                // Queue destination for resolution
                if (breakOrCont == Continue)
                    comp->addPendingContinue(dest, pc);
                else
                    comp->addPendingBreak(dest, pc);

                return;
            } // if matching destination..
        }
    }

    assert (!"Huh? Unable to find continue/break target in the nest stack");
}

void ContinueNode::generateExecCode(CompileState* comp)
{
    generateDebugInfoIfNeeded(comp);
    Node* dest = comp->resolveContinueLabel(ident);
    if (!dest) {
        if (ident.isEmpty())
            emitSyntaxError(comp, this, "Illegal continue without target outside a loop.");
        else
            emitSyntaxError(comp, this, "Invalid label in continue.");
    } else {
        // Continue can only be used for a loop
        if (dest->isIterationStatement()) {
            handleJumpOut(comp, dest, Continue);
        } else {
            emitSyntaxError(comp, this, "Invalid continue target; must be a loop.");
        }
    }
}

void BreakNode::generateExecCode(CompileState* comp)
{
    generateDebugInfoIfNeeded(comp);
    Node* dest = comp->resolveBreakLabel(ident);
    if (!dest) {
        if (ident.isEmpty())
            emitSyntaxError(comp, this, "Illegal break without target outside a loop or switch.");
        else
            emitSyntaxError(comp, this, "Invalid label in break.");
    } else {
        handleJumpOut(comp, dest, Break);
    }
}

void ReturnNode::generateExecCode(CompileState* comp)
{
    generateDebugInfoIfNeeded(comp);
    OpValue arg;

    // Return is invalid in non-function..
    if (comp->codeType() != FunctionCode) {
        emitSyntaxError(comp, this, "Invalid return.");
        return;
    }

    if (!value)
        arg = OpValue::immValue(jsUndefined());
    else
        arg = value->generateEvalCode(comp);

    if (!comp->inTryFinally())
        generateExitContextIfNeeded(comp);

    CodeGen::emitOp(comp, comp->inTryFinally() ? Op_ReturnInTryFinally : Op_Return, 0, &arg);
}

void WithNode::generateExecCode(CompileState* comp)
{
    generateDebugInfoIfNeeded(comp);
    // ### this may be too conservative --- it only applies if there is
    // a function call within
    comp->setNeedsClosures();

    OpValue scopeObj = expr->generateEvalCode(comp);

    comp->pushNest(CompileState::Scope, this);
    CodeGen::emitOp(comp, Op_PushScope, 0, &scopeObj);

    statement->generateExecCode(comp);

    CodeGen::emitOp(comp, Op_PopScope, 0);
    comp->popNest();
}

void LabelNode::generateExecCode(CompileState* comp)
{
    if (!comp->pushLabel(label)) {
        emitSyntaxError(comp, this, "Duplicated label found.");
        return;
    }

    if (!statement->isLabelNode()) { // we're the last label..
        comp->pushNest(CompileState::ContBreakTarget, statement.get());
        comp->bindLabels(statement.get());
    }

    // Generate code for stuff inside the label...
    statement->generateExecCode(comp);

    // Fix up any breaks..
    if (!statement->isLabelNode()) {
        comp->popNest();
        comp->resolvePendingBreaks(statement.get(), CodeGen::nextPC(comp));
    }

    comp->popLabel();
}

void ThrowNode::generateExecCode(CompileState* comp)
{
    generateDebugInfoIfNeeded(comp);
    OpValue projectile = expr->generateEvalCode(comp);
    CodeGen::emitOp(comp, Op_Throw, 0, &projectile);
}

void TryNode::generateExecCode(CompileState* comp)
{
    generateDebugInfoIfNeeded(comp);
    // ### this may be too conservative --- it only applies if there is
    // a function call within the catch
    comp->setNeedsClosures();

    // Set the catch handler, run the try clause, pop the try handler..
    Addr setCatchHandler = CodeGen::emitOp(comp, Op_PushExceptionHandler, 0, OpValue::dummyAddr());
    comp->pushNest(finallyBlock ? CompileState::TryFinally : CompileState::OtherCleanup);

    tryBlock->generateExecCode(comp);

    CodeGen::emitOp(comp, Op_PopExceptionHandler);
    comp->popNest();

    // Jump over the catch if try is OK
    Addr jumpOverCatch = 0;
    if (catchBlock)
        jumpOverCatch = CodeGen::emitOp(comp, Op_Jump, 0, OpValue::dummyAddr());

    // Exceptions would go here --- either in a catch or a finally.
    CodeGen::patchJumpToNext(comp, setCatchHandler, 0);

    Addr catchToFinallyEH = 0;
    if (catchBlock) {
        // If there is a finally block, that acts as an exception handler for the catch;
        // we need to set it before entering the catch scope, so the cleanup entries for that
        // are on top. Also, that's needed because if the inside raised a non-exception
        // continuation, EnterCatch will re-raise it.
        if (finallyBlock) {
            catchToFinallyEH = CodeGen::emitOp(comp, Op_PushExceptionHandler, 0, OpValue::dummyAddr());
            comp->pushNest(CompileState::TryFinally);
        }

        // Emit the catch.. Note: the unwinder has already popped the catch handler entry,
        // but the exception object is still set, since we need to make a scope for it.
        // EnterCatch would do that for us, given the name
        OpValue catchVar = OpValue::immIdent(&exceptionIdent);
        CodeGen::emitOp(comp, Op_EnterCatch, 0, &catchVar);
        comp->pushNest(CompileState::Scope);

        catchBlock->generateExecCode(comp);

        // If needed, cleanup the binding to finally, and always cleans the catch scope
        CodeGen::emitOp(comp, Op_ExitCatch);
        comp->popNest();

        if (finallyBlock) {
            CodeGen::emitOp(comp, Op_PopExceptionHandler);
            comp->popNest();
        }

        // after an OK 'try', we always go to finally, if any, which needs an op if there is a catch block
        CodeGen::patchJumpToNext(comp, jumpOverCatch, 0);
    }


    if (finallyBlock) {
        if (catchBlock) // if a catch was using us an EH, patch that instruction to here
            CodeGen::patchJumpToNext(comp, catchToFinallyEH, 0);

        CodeGen::emitOp(comp, Op_DeferCompletion);
        comp->pushNest(CompileState::OtherCleanup);

        finallyBlock->generateExecCode(comp);

        OpValue otherTryFinally = OpValue::immBool(comp->inTryFinally());

        if (exitContextNeeded(comp)) {
            OpValue ourNode = OpValue::immNode(comp->functionBody());
            CodeGen::emitOp(comp, Op_ReactivateCompletionDebug, 0, &otherTryFinally, &ourNode);
        } else {
            CodeGen::emitOp(comp, Op_ReactivateCompletion, 0, &otherTryFinally);
        }
        comp->popNest();
    }
}

void FunctionBodyNode::generateExecCode(CompileState* comp)
{
    // Load scope, global and 'this' pointers.
    OpValue scopeVal,  scopeReg,
            globalVal, globalReg,
            thisVal,   thisReg;

    comp->requestTemporary(OpType_value, &scopeVal,  &scopeReg);
    comp->requestTemporary(OpType_value, &globalVal, &globalReg);
    comp->requestTemporary(OpType_value, &thisVal,   &thisReg);

    CodeGen::emitOp(comp, Op_Preamble, 0, &scopeReg, &globalReg, &thisReg);

    comp->setPreloadRegs(&scopeVal, &globalVal, &thisVal);

    OpValue evalResReg, evalResVal;
    if (comp->codeType() != FunctionCode) {
        comp->requestTemporary(OpType_value, &evalResVal, &evalResReg);
        comp->setEvalResultRegister(&evalResReg);

        // There is no need to initialize this as everything will be set to undefined anyway
    } else {
        if (comp->compileType() == Debug) {
            OpValue ourNode = OpValue::immNode(this);
            CodeGen::emitOp(comp, Op_EnterDebugContext, 0, &ourNode);
        }
    }

    // Set unwind..
    Addr unwind = CodeGen::emitOp(comp, Op_PushExceptionHandler, 0, OpValue::dummyAddr());

    // Generate body...
    BlockNode::generateExecCode(comp);

    // Make sure we exit!
    if (comp->codeType() != FunctionCode) {
        CodeGen::emitOp(comp, Op_Return, 0, &evalResVal);
    } else {
        generateExitContextIfNeeded(comp);
        CodeGen::emitOp(comp, Op_Exit);
    }

    // Unwind stuff..
    CodeGen::patchJumpToNext(comp, unwind, 0);
    generateExitContextIfNeeded(comp);
    CodeGen::emitOp(comp, Op_PropagateException);
}

void SwitchNode::generateExecCode(CompileState* comp)
{
    generateDebugInfoIfNeeded(comp);
    CaseBlockNode* caseBlock = this->block.get();

    // The code we produce has 2 stages: first, we emit all the conditionals, and pick
    // the label to jump to (with the jump to the default being last),
    // then we just emit all the clauses in the row. The breaks will be
    // resolved at the end --- for that, we bind ourselves for label'less break.
    comp->pushNest(CompileState::ContBreakTarget, this);
    comp->pushDefaultBreak(this);

    // What we compare with
    OpValue switchOn = expr->generateEvalCode(comp);

    WTF::Vector<Addr> list1jumps;
    WTF::Vector<Addr> list2jumps;
    Addr defJump;

    // Jumps for list 1..
    for (ClauseListNode* iter = caseBlock->list1.get(); iter; iter = iter->next.get()) {
        OpValue ref = iter->clause->expr->generateEvalCode(comp);
        OpValue match;
        CodeGen::emitOp(comp, Op_StrEq, &match, &switchOn, &ref);

        Addr jumpToClause = CodeGen::emitOp(comp, Op_IfJump, 0, &match, OpValue::dummyAddr());
        list1jumps.append(jumpToClause);
    }

    // Jumps for list 2..
    for (ClauseListNode* iter = caseBlock->list2.get(); iter; iter = iter->next.get()) {
        OpValue ref = iter->clause->expr->generateEvalCode(comp);
        OpValue match;
        CodeGen::emitOp(comp, Op_StrEq, &match, &switchOn, &ref);

        Addr jumpToClause = CodeGen::emitOp(comp, Op_IfJump, 0, &match, OpValue::dummyAddr());
        list2jumps.append(jumpToClause);
    }

    // Jump to default (or after, if there is no default)
    defJump = CodeGen::emitOp(comp, Op_Jump, 0, OpValue::dummyAddr());

    // Now, we can actually emit the bodies, fixing the addresses as we go
    int p = 0;
    for (ClauseListNode* iter = caseBlock->list1.get(); iter; iter = iter->next.get()) {
        CodeGen::patchJumpToNext(comp, list1jumps[p], 1);
        if (iter->clause->source)
            iter->clause->source->generateExecCode(comp);
        ++p;
    }

    if (caseBlock->def) {
        CodeGen::patchJumpToNext(comp, defJump, 0);
        if (caseBlock->def->source)
            caseBlock->def->source->generateExecCode(comp);
    }

    p = 0;
    for (ClauseListNode* iter = caseBlock->list2.get(); iter; iter = iter->next.get()) {
        CodeGen::patchJumpToNext(comp, list2jumps[p], 1);
        if (iter->clause->source)
            iter->clause->source->generateExecCode(comp);
        ++p;
    }

    // If we didn't have a default, that jump is to here..
    if (!caseBlock->def)
        CodeGen::patchJumpToNext(comp, defJump, 0);

    // Breaks should go after us..
    comp->popDefaultBreak();
    comp->popNest();
    comp->resolvePendingBreaks(this, CodeGen::nextPC(comp));
}

void ImportStatement::generateExecCode(CompileState*)
{} // handled as a declaration..


}

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on; hl c++;
