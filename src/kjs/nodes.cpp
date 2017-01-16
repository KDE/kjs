/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2002 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003, 2004, 2005, 2006, 2007 Apple Inc. All rights reserved.
 *  Copyright (C) 2007 Maksim Orlovich (maksim@kde.org)
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

#include "nodes.h"
#include "scriptfunction.h"
#include "CompileState.h"

#include <math.h>
#include <stdio.h>
#ifdef KJS_DEBUG_MEM
#include <typeinfo>
#endif

//#include <iostream>

#include "debugger.h"
#include "function_object.h"
#include "lexer.h"
#include "operations.h"
#include "package.h"
#include "PropertyNameArray.h"
#include <wtf/AlwaysInline.h>
#include <wtf/Assertions.h>
#include <wtf/HashSet.h>
#include <wtf/HashCountedSet.h>
#include <wtf/MathExtras.h>

#include "bytecode/machine.h"

namespace KJS
{

// ------------------------------ Node -----------------------------------------

#ifndef NDEBUG
struct NodeCounter {
    static unsigned count;
    ~NodeCounter()
    {
        if (count) {
            fprintf(stderr, "LEAK: %d KJS::Node\n", count);
        }
    }
};
unsigned NodeCounter::count = 0;
static NodeCounter nodeCounter;
#endif

static HashSet<Node *> *newNodes;
static HashCountedSet<Node *> *nodeExtraRefCounts;

Node::Node()
{
#ifndef NDEBUG
    ++NodeCounter::count;
#endif
    m_line = lexer().lineNo();
    if (!newNodes) {
        newNodes = new HashSet<Node *>;
    }
    newNodes->add(this);
}

Node::~Node()
{
#ifndef NDEBUG
    --NodeCounter::count;
#endif
}

void Node::ref()
{
    // bumping from 0 to 1 is just removing from the new nodes set
    if (newNodes) {
        HashSet<Node *>::iterator it = newNodes->find(this);
        if (it != newNodes->end()) {
            newNodes->remove(it);
            ASSERT(!nodeExtraRefCounts || !nodeExtraRefCounts->contains(this));
            return;
        }
    }

    ASSERT(!newNodes || !newNodes->contains(this));

    if (!nodeExtraRefCounts) {
        nodeExtraRefCounts = new HashCountedSet<Node *>;
    }
    nodeExtraRefCounts->add(this);
}

void Node::deref()
{
    ASSERT(!newNodes || !newNodes->contains(this));

    if (!nodeExtraRefCounts) {
        delete this;
        return;
    }

    HashCountedSet<Node *>::iterator it = nodeExtraRefCounts->find(this);
    if (it == nodeExtraRefCounts->end()) {
        delete this;
    } else {
        nodeExtraRefCounts->remove(it);
    }
}

unsigned Node::refcount()
{
    if (newNodes && newNodes->contains(this)) {
        ASSERT(!nodeExtraRefCounts || !nodeExtraRefCounts->contains(this));
        return 0;
    }

    ASSERT(!newNodes || !newNodes->contains(this));

    if (!nodeExtraRefCounts) {
        return 1;
    }

    return 1 + nodeExtraRefCounts->count(this);
}

void Node::clearNewNodes()
{
    if (!newNodes) {
        return;
    }

#ifndef NDEBUG
    HashSet<Node *>::iterator end = newNodes->end();
    for (HashSet<Node *>::iterator it = newNodes->begin(); it != end; ++it) {
        ASSERT(!nodeExtraRefCounts || !nodeExtraRefCounts->contains(*it));
    }
#endif
    deleteAllValues(*newNodes);
    delete newNodes;
    newNodes = nullptr;
}

static void substitute(UString &string, const UString &substring)
{
    int position = string.find("%s");
    assert(position != -1);
    UString newString = string.substr(0, position);
    newString.append(substring);
    newString.append(string.substr(position + 2));
    string = newString;
}

static inline int currentSourceId(ExecState *exec)
{
    return exec->currentBody()->sourceId();
}

static inline const UString &currentSourceURL(ExecState *exec)
{
    return exec->currentBody()->sourceURL();
}

JSValue *Node::throwError(ExecState *exec, ErrorType e, const UString &msg)
{
    return KJS::throwError(exec, e, msg, lineNo(), currentSourceId(exec), currentSourceURL(exec));
}

JSValue *Node::throwError(ExecState *exec, ErrorType e, const UString &msg, const Identifier &label)
{
    UString message = msg;
    substitute(message, label.ustring());
    return KJS::throwError(exec, e, message, lineNo(), currentSourceId(exec), currentSourceURL(exec));
}

JSValue *Node::throwUndefinedVariableError(ExecState *exec, const Identifier &ident)
{
    return throwError(exec, ReferenceError, "Cannot find variable: %s", ident);
}

Node *Node::nodeInsideAllParens()
{
    return this;
}

class VarDeclVisitor: public NodeVisitor
{
private:
    ExecState *m_exec;
public:
    VarDeclVisitor(ExecState *exec) : m_exec(exec)
    {}

    virtual Node *visit(Node *node)
    {
        node->processVarDecl(m_exec);

        //Do not recurse inside function bodies, or things that
        // syntactically can't contain declarations
        if (!node->scanForDeclarations()) {
            return nullptr;
        }

        return NodeVisitor::visit(node);
    }
};

class FuncDeclVisitor: public NodeVisitor
{
private:
    ExecState *m_exec;
public:
    FuncDeclVisitor(ExecState *exec) : m_exec(exec)
    {}

    virtual Node *visit(Node *node)
    {
        node->processFuncDecl(m_exec);

        if (!node->scanForDeclarations()) {
            return nullptr;
        }

        return NodeVisitor::visit(node);
    }
};

void Node::processDecls(ExecState *exec)
{
    VarDeclVisitor vVisit(exec);
    vVisit.visit(this);

    FuncDeclVisitor fVisit(exec);
    fVisit.visit(this);
}

void Node::processVarDecl(ExecState *)
{}

void Node::processFuncDecl(ExecState *)
{}

// ------------------------------ NodeVisitor ----------------------------------
Node *NodeVisitor::visit(Node *node)
{
    node->recurseVisit(this);
    return nullptr;
}

// ------------------------------ StatementNode --------------------------------

StatementNode::StatementNode()
    : m_lastLine(-1)
{
    m_line = -1;
}

void StatementNode::setLoc(int firstLine, int lastLine) const
{
    m_line = firstLine;
    m_lastLine = lastLine;
}

void StatementNode::hitStatement(ExecState *exec)
{
    // The debugger is always non-zero here, since otherwise this won't be involved
    exec->dynamicInterpreter()->debugger()->reportAtStatement(exec, currentSourceId(exec), firstLine(), lastLine());
}

// ------------------------------ GroupNode ------------------------------------

Node *GroupNode::nodeInsideAllParens()
{
    Node *n = this;
    do {
        n = static_cast<GroupNode *>(n)->group.get();
    } while (n->isGroupNode());
    return n;
}

void GroupNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, group);
}

// ------------------------------ ElementNode ----------------------------------

void ElementNode::breakCycle()
{
    next = nullptr;
}

void ElementNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, next);
    recurseVisitLink(visitor, node);
}

// ------------------------------ ArrayNode ------------------------------------

void ArrayNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, element);
}

// ------------------------------ ObjectLiteralNode ----------------------------

void ObjectLiteralNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, list);
}

// ------------------------------ PropertyListNode -----------------------------

void PropertyListNode::breakCycle()
{
    next = nullptr;
}

void PropertyListNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, node);
    recurseVisitLink(visitor, next);
}

// ------------------------------ PropertyNode -----------------------------
void PropertyNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, name);
    recurseVisitLink(visitor, assign);
}

// ------------------------------ BracketAccessorNode --------------------------------

void BracketAccessorNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr1);
    recurseVisitLink(visitor, expr2);
}

// ------------------------------ DotAccessorNode --------------------------------

void DotAccessorNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
}

// ------------------------------ ArgumentListNode -----------------------------

void ArgumentListNode::breakCycle()
{
    next = nullptr;
}

void ArgumentListNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, next);
    recurseVisitLink(visitor, expr);
}

// ------------------------------ ArgumentsNode --------------------------------

void ArgumentsNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, list);
}

// ------------------------------ NewExprNode ----------------------------------

void NewExprNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
    recurseVisitLink(visitor, args);
}

// ------------------------------ FunctionCallValueNode ------------------------

void FunctionCallValueNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
    recurseVisitLink(visitor, args);
}

// ------------------------------ FunctionCallRerefenceNode --------------------

void FunctionCallReferenceNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
    recurseVisitLink(visitor, args);
}

// ------------------------------ PostfixNode ----------------------------------

void PostfixNode::recurseVisit(NodeVisitor *visitor)
{
    Node::recurseVisitLink(visitor, m_loc);
}

// ------------------------------ DeleteReferenceNode -------------------------------

void DeleteReferenceNode::recurseVisit(NodeVisitor *visitor)
{
    Node::recurseVisitLink(visitor, loc);
}

// ------------------------------ DeleteValueNode -----------------------------------

void DeleteValueNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, m_expr);
}

// ------------------------------ VoidNode -------------------------------------

void VoidNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
}

// ------------------------------ TypeOfVarNode -----------------------------------

void TypeOfVarNode::recurseVisit(NodeVisitor *visitor)
{
    Node::recurseVisitLink(visitor, loc);
}

// ------------------------------ TypeOfValueNode -----------------------------------

void TypeOfValueNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, m_expr);
}

// ------------------------------ PrefixNode ----------------------------------------

void PrefixNode::recurseVisit(NodeVisitor *visitor)
{
    Node::recurseVisitLink(visitor, m_loc);
}

// ------------------------------ UnaryPlusNode --------------------------------

void UnaryPlusNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
}

// ------------------------------ NegateNode -----------------------------------

void NegateNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
}

// ------------------------------ BitwiseNotNode -------------------------------

void BitwiseNotNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
}

// ------------------------------ LogicalNotNode -------------------------------

void LogicalNotNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
}

// ------------------------ BinaryOperatorNode -------------------------------

void BinaryOperatorNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr1);
    recurseVisitLink(visitor, expr2);
}

// ------------------------------ BinaryLogicalNode ----------------------------

void BinaryLogicalNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr1);
    recurseVisitLink(visitor, expr2);
}

// ------------------------------ ConditionalNode ------------------------------

void ConditionalNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, logical);
    recurseVisitLink(visitor, expr1);
    recurseVisitLink(visitor, expr2);
}

// ------------------------------ AssignNode -----------------------------------

void AssignNode::recurseVisit(NodeVisitor *visitor)
{
    Node::recurseVisitLink(visitor, m_loc);
    Node::recurseVisitLink(visitor, m_right);
}

// ------------------------------ CommaNode ------------------------------------

void CommaNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr1);
    recurseVisitLink(visitor, expr2);
}

// ------------------------------ AssignExprNode -------------------------------

void AssignExprNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
}

// ------------------------------ VarDeclNode ----------------------------------

VarDeclNode::VarDeclNode(const Identifier &id, AssignExprNode *in, Type t)
    : varType(t), ident(id), init(in)
{
}

#if 0
// ECMA 12.2
JSValue *VarDeclNode::evaluate(ExecState *exec)
{
    JSObject *variable = exec->variableObject();

    JSValue *val;
    if (init) {
        val = init->evaluate(exec);
        KJS_CHECKEXCEPTIONVALUE
    } else {
        // already declared? - check with getDirect so you can override
        // built-in properties of the global object with var declarations.
        // Also check for 'arguments' property. The 'arguments' cannot be found with
        // getDirect, because it's created lazily by
        // ActivationImp::getOwnPropertySlot.
        // Since variable declarations are always in function scope, 'variable'
        // will always contain instance of ActivationImp and ActivationImp will
        // always have 'arguments' property
        if (variable->getDirect(ident) || ident == exec->propertyNames().arguments) {
            return 0;
        }
        val = jsUndefined();
    }

#ifdef KJS_VERBOSE
    printInfo(exec, (UString("new variable ") + ident.ustring()).cstring().c_str(), val);
#endif
    // We use Internal to bypass all checks in derived objects, e.g. so that
    // "var location" creates a dynamic property instead of activating window.location.
    int flags = Internal;
    if (exec->codeType() != EvalCode) {
        flags |= DontDelete;
    }
    if (varType == VarDeclNode::Constant) {
        flags |= ReadOnly;
    }
    variable->put(exec, ident, val, flags);

    return 0; //No useful value, not a true expr
}
#endif

void VarDeclNode::processVarDecl(ExecState *exec)
{
    JSObject *variable = exec->variableObject();

    // First, determine which flags we want to use..
    int flags = DontDelete;
    if (varType == VarDeclNode::Constant) {
        flags |= ReadOnly;
    }

    // Are we inside a function? If so, we fill in the symbol table
    switch (exec->codeType()) {
    case FunctionCode:
        // Inside a function, we're just computing static information.
        // so, just fill in the symbol table.
        exec->currentBody()->addVarDecl(ident, flags, exec);
        return;
    case EvalCode:
        // eval-injected variables can be deleted..
        flags &= ~DontDelete;

        // If a variable by this name already exists, don't clobber it -
        // eval may be trying to inject a variable that already exists..
        if (!variable->hasProperty(exec, ident)) {
            variable->put(exec, ident, jsUndefined(), flags);
            // eval injected a new local into scope! Better mark that down,
            // so that NonLocalResolver stops skipping the local scope
            variable->setLocalInjected();
        }
        break;
    case GlobalCode:
        // If a variable by this name already exists, don't clobber it -
        // ### I am not sue this is needed for GlobalCode
        if (!variable->hasProperty(exec, ident)) {
            variable->put(exec, ident, jsUndefined(), flags);
        }
    };
}

void VarDeclNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, init);
}

// ------------------------------ VarDeclListNode ------------------------------

void VarDeclListNode::breakCycle()
{
    next = nullptr;
}

void VarDeclListNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, var);
    recurseVisitLink(visitor, next);
}

// ------------------------------ VarStatementNode -----------------------------

void VarStatementNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, next);
}

// ------------------------------ BlockNode ------------------------------------

BlockNode::BlockNode(SourceElementsNode *s)
{
    if (s) {
        source = s->next.release();
        Parser::removeNodeCycle(source.get());
        setLoc(s->firstLine(), s->lastLine());
    } else {
        source = nullptr;
    }
}

void BlockNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, source);
}

// ------------------------------ ExprStatementNode ----------------------------

void ExprStatementNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
}

// ------------------------------ IfNode ---------------------------------------

void IfNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
    recurseVisitLink(visitor, statement1);
    recurseVisitLink(visitor, statement2);
}

// ------------------------------ DoWhileNode ----------------------------------

void DoWhileNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
    recurseVisitLink(visitor, statement);
}

// ------------------------------ WhileNode ------------------------------------

void WhileNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
    recurseVisitLink(visitor, statement);
}

// ------------------------------ ForNode --------------------------------------

void ForNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr1);
    recurseVisitLink(visitor, expr2);
    recurseVisitLink(visitor, expr3);
    recurseVisitLink(visitor, statement);
}

// ------------------------------ ForInNode ------------------------------------

ForInNode::ForInNode(Node *l, Node *e, StatementNode *s)
    : init(nullptr), lexpr(l), expr(e), varDecl(nullptr), statement(s)
{
}

ForInNode::ForInNode(const Identifier &i, AssignExprNode *in, Node *e, StatementNode *s)
    : ident(i), init(in), expr(e), statement(s)
{
    // for( var foo = bar in baz )
    varDecl = new VarDeclNode(ident, init.get(), VarDeclNode::Variable);
    lexpr   = new VarAccessNode(ident);
}

void ForInNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, init);
    recurseVisitLink(visitor, lexpr);
    recurseVisitLink(visitor, expr);
    recurseVisitLink(visitor, varDecl);
    recurseVisitLink(visitor, statement);
}

// ------------------------------ ReturnNode -----------------------------------

void ReturnNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, value);
}

// ------------------------------ WithNode -------------------------------------

void WithNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
    recurseVisitLink(visitor, statement);
}

// ------------------------------ CaseClauseNode -------------------------------

void CaseClauseNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
    recurseVisitLink(visitor, source);
}

// ------------------------------ ClauseListNode -------------------------------

void ClauseListNode::breakCycle()
{
    next = nullptr;
}

void ClauseListNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, clause);
    recurseVisitLink(visitor, next);
}

// ------------------------------ CaseBlockNode --------------------------------

CaseBlockNode::CaseBlockNode(ClauseListNode *l1, CaseClauseNode *d,
                             ClauseListNode *l2)
{
    if (l1) {
        list1 = l1->next.release();
        Parser::removeNodeCycle(list1.get());
    } else {
        list1 = nullptr;
    }

    def = d;

    if (l2) {
        list2 = l2->next.release();
        Parser::removeNodeCycle(list2.get());
    } else {
        list2 = nullptr;
    }
}

void CaseBlockNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, list1);
    recurseVisitLink(visitor, def);
    recurseVisitLink(visitor, list2);
}

// ------------------------------ SwitchNode -----------------------------------

void SwitchNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
    recurseVisitLink(visitor, block);
}

// ------------------------------ LabelNode ------------------------------------

void LabelNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, statement);
}

// ------------------------------ ThrowNode ------------------------------------

void ThrowNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, expr);
}

// ------------------------------ TryNode --------------------------------------

void TryNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, tryBlock);
    recurseVisitLink(visitor, catchBlock);
    recurseVisitLink(visitor, finallyBlock);
}

// ------------------------------ ParameterNode --------------------------------

void ParameterNode::breakCycle()
{
    next = nullptr;
}

void ParameterNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, next);
}

// ------------------------------ FunctionBodyNode -----------------------------

FunctionBodyNode::FunctionBodyNode(SourceElementsNode *s)
    : BlockNode(s)
    , m_sourceURL(lexer().sourceURL())
    , m_sourceId(parser().sourceId())
    , m_compType(NotCompiled)
    , m_flags(parser().popFunctionContext())
{
    setLoc(-1, -1);
}

void FunctionBodyNode::addVarDecl(const Identifier &ident, int attr, ExecState *exec)
{
    // There is one nasty special case: ignore a 'var' declaration of 'arguments';
    // it effectively doesn't do anything since the magic 'arguments' is already
    // in scope anyway, and if we allocated a local, we would have to worry about
    // keeping track of whether it was initialized or not on what is supposed to be the
    // fast path. So we just make this go through the property map instead.
    // Note that this does not matter for parameters or function declarations,
    // since those overwrite the magic 'arguments' anyway.
    if (ident == exec->propertyNames().arguments) {
        return;
    }

    (void)addSymbol(ident, attr);
}

void FunctionBodyNode::addFunDecl(const Identifier &ident, int attr, FuncDeclNode *funcDecl)
{
    m_functionLocals.append(addSymbol(ident, attr, funcDecl));
}

void FunctionBodyNode::reserveSlot(size_t id, bool shouldMark)
{
    ASSERT(id == m_symbolList.size());
    m_symbolList.append(SymbolInfo(shouldMark ? 0 : DontMark, nullptr));
}

size_t FunctionBodyNode::addSymbol(const Identifier &ident, int flags, FuncDeclNode *funcDecl)
{
    // We get symbols in the order specified in 10.1.3, but sometimes
    // the later ones are supposed to lose. This -mostly- does not
    // matter for us --- we primarily concern ourselves with name/ID
    // mapping, but there is an issue of attributes and funcDecl's.
    // However, the only flag that matters here is ReadOnly --
    // everything else just has DontDelete set; and it's from const,
    // so we can just ignore it on repetitions, since var/const should lose
    // and are at the end.
    //
    // And for funcDecl, since functions win over everything, we always set it if non-zero
    size_t oldId = m_symbolTable.get(ident.ustring().rep());
    if (oldId != missingSymbolMarker()) {
        if (funcDecl) {
            m_symbolList[oldId].funcDecl = funcDecl;
        }
        return oldId;
    }

    size_t id = m_symbolList.size();         //First entry gets 0, etc.
    m_symbolTable.set(ident.ustring().rep(), id);
    m_symbolList.append(SymbolInfo(flags, funcDecl));
    return id;
}

void FunctionBodyNode::addSymbolOverwriteID(size_t id, const Identifier &ident, int flags)
{
    ASSERT(id == m_symbolList.size());

    // Remove previous one, if any
    size_t oldId = m_symbolTable.get(ident.ustring().rep());
    if (oldId != missingSymbolMarker()) {
        m_symbolList[oldId].attr = DontMark;
    }

    // Add a new one
    m_symbolTable.set(ident.ustring().rep(), id);
    m_symbolList.append(SymbolInfo(flags, nullptr));
}

void FunctionBodyNode::addParam(const Identifier &ident)
{
    m_paramList.append(ident);
}

Completion FunctionBodyNode::execute(ExecState *exec)
{
    CodeType    ctype   = exec->codeType();
    CompileType cmpType = exec->dynamicInterpreter()->debugger() ? Debug : Release;
    compileIfNeeded(ctype, cmpType);
    ASSERT(ctype != FunctionCode);

    LocalStorage      *store = new LocalStorage();
    LocalStorageEntry *regs;

    // Allocate enough space, and make sure to initialize things so we don't mark garbage
    store->resize(m_symbolList.size());
    regs = store->data();
    for (size_t c = 0; c < m_symbolList.size(); ++c) {
        regs[c].val.valueVal = jsUndefined();
        regs[c].attributes   = m_symbolList[c].attr;
    }

    exec->initLocalStorage(regs, m_symbolList.size());

    JSValue *val = Machine::runBlock(exec, m_compiledCode);

    Completion result;
    if (exec->hadException()) {
        result = Completion(Throw, exec->exception());
    } else {
        result = Completion(Normal, val);
    }

    exec->initLocalStorage(nullptr, 0);
    delete store;
    exec->clearException();

    return result;
}

void FunctionBodyNode::compile(CodeType ctype, CompileType compType)
{
    m_compType = compType;

    CompileState comp(ctype, compType, this, m_symbolList.size());
    generateExecCode(&comp);
    m_tearOffAtEnd = comp.needsClosures();

#if 0
    fprintf(stderr, "\n\n");
    fprintf(stderr, "\n---------------------------------\n\n");
    fprintf(stderr, "%s", toString().ascii());
    fprintf(stderr, "\n---------------------------------\n\n");
    CodeGen::disassembleBlock(m_compiledCode);
    fprintf(stderr, "\n---------------------------------\n\n");
#endif
}

// ------------------------------ FuncDeclNode ---------------------------------

// ECMA 13
void FuncDeclNode::processFuncDecl(ExecState *exec)
{
    // See whether we just need to fill in the symbol table,
    // or actually fiddle with objects.
    int flags = Internal | DontDelete;
    switch (exec->codeType()) {
    case FunctionCode:
        // Inside a function, just need symbol info
        exec->currentBody()->addFunDecl(ident, flags, this);
        return;
    case EvalCode:
        // eval-injected symbols can be deleted...
        flags &= ~DontDelete;

        // eval injected a new local into scope! Better mark that down,
        // so that NonLocalResolver stops skipping the local scope
        exec->variableObject()->setLocalInjected();

    // fallthrough intentional
    case GlobalCode:
        exec->variableObject()->put(exec, ident, makeFunctionObject(exec), flags);
    };
}

void FuncDeclNode::addParams()
{
    for (ParameterNode *p = param.get(); p != nullptr; p = p->nextParam()) {
        body->addParam(p->ident());
    }
}

FunctionImp *FuncDeclNode::makeFunctionObject(ExecState *exec)
{
    // TODO: let this be an object with [[Class]] property "Function"
    FunctionImp *func = new FunctionImp(exec, ident, body.get(), exec->scopeChain());

    JSObject *proto = exec->lexicalInterpreter()->builtinObject()->construct(exec, List::empty());
    proto->put(exec, exec->propertyNames().constructor, func, DontEnum);
    // ECMA Edition 5.1r6 - 15.3.5.2 - [[Writable]]: true, [[Enumerable]]: false, [[Configurable]]: false
    func->put(exec, exec->propertyNames().prototype, proto, Internal | DontDelete | DontEnum);

    func->put(exec, exec->propertyNames().length, jsNumber(body->numParams()), ReadOnly | DontDelete | DontEnum);

    return func;
}

void FuncDeclNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, param);
    recurseVisitLink(visitor, body);
}

// ------------------------------ FuncExprNode ---------------------------------

void FuncExprNode::addParams()
{
    for (ParameterNode *p = param.get(); p != nullptr; p = p->nextParam()) {
        body->addParam(p->ident());
    }
}

void FuncExprNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, param);
    recurseVisitLink(visitor, body);
}

// ------------------------------ SourceElementsNode ---------------------------

SourceElementsNode::SourceElementsNode(StatementNode *s1)
    : node(s1), next(this)
{
    Parser::noteNodeCycle(this);
    setLoc(s1->firstLine(), s1->lastLine());
}

SourceElementsNode::SourceElementsNode(SourceElementsNode *s1, StatementNode *s2)
    : node(s2), next(s1->next)
{
    s1->next = this;
    setLoc(s1->firstLine(), s2->lastLine());
}

void SourceElementsNode::breakCycle()
{
    next = nullptr;
}

void SourceElementsNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, node);
    recurseVisitLink(visitor, next);
}

// ------------------------------ ProgramNode ----------------------------------

ProgramNode::ProgramNode(SourceElementsNode *s) : FunctionBodyNode(s)
{
}

// ------------------------------ PackageNameNode ------------------------------
void PackageNameNode::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, names);
}

Completion PackageNameNode::loadSymbol(ExecState *exec, bool wildcard)
{
    Package *basePackage;
    JSObject *baseObject;
    if (names) {
        PackageObject *pobj = names->resolvePackage(exec);
        if (pobj == nullptr) {
            return Completion(Normal);
        }
        basePackage = pobj->package();
        baseObject = pobj;
    } else {
        Interpreter *ip = exec->lexicalInterpreter();
        basePackage = ip->globalPackage();
        baseObject = ip->globalObject();
    }

    if (wildcard) {
        // if a .* is specified the last identifier should
        // denote another package name
        PackageObject *pobj = resolvePackage(exec, baseObject, basePackage);
        if (!pobj) {
            return Completion(Normal);
        }
        basePackage = pobj->package();
        baseObject = pobj;
        basePackage->loadAllSymbols(exec, baseObject);
    } else {
        basePackage->loadSymbol(exec, baseObject, id);
    }

    return Completion(Normal);
}

PackageObject *PackageNameNode::resolvePackage(ExecState *exec)
{
    JSObject *baseObject;
    Package *basePackage;
    if (names) {
        PackageObject *basePackageObject = names->resolvePackage(exec);
        if (basePackageObject == nullptr) {
            return nullptr;
        }
        baseObject = basePackageObject;
        basePackage = basePackageObject->package();
    } else {
        // first identifier is looked up in global object
        Interpreter *ip = exec->lexicalInterpreter();
        baseObject = ip->globalObject();
        basePackage = ip->globalPackage();
    }

    return resolvePackage(exec, baseObject, basePackage);
}

PackageObject *PackageNameNode::resolvePackage(ExecState *exec,
        JSObject *baseObject,
        Package *basePackage)
{
    PackageObject *res = nullptr;

    // Let's see whether the package was already resolved previously.
    JSValue *v = baseObject->get(exec, id);
    if (v && !v->isUndefined()) {
        if (!v->isObject()) {
            // Symbol conflict
            throwError(exec, GeneralError, "Invalid type of package %s", id);
            return nullptr;
        }
        res = static_cast<PackageObject *>(v);
    } else {
        UString err;
        Package *newBase = basePackage->loadSubPackage(id, &err);
        if (newBase == nullptr) {
            if (err.isEmpty()) {
                throwError(exec, GeneralError, "Package not found");
            } else {
                throwError(exec, GeneralError, err);
            }
            return nullptr;
        }
        res = new PackageObject(newBase);
        baseObject->put(exec, id, res);
    }

    return res;
}

void ImportStatement::processVarDecl(ExecState *exec)
{
    // error out if package support is not activated
    Package *glob = exec->lexicalInterpreter()->globalPackage();
    if (!glob) {
        throwError(exec, GeneralError,
                   "Package support disabled. Import failed.");
        return;
    }

    // also error out if not used on top-level
    if (exec->codeType() != GlobalCode) {
        throwError(exec, GeneralError,
                   "Package imports may only occur at top level.");
        return;
    }

    name->loadSymbol(exec, wld);
}

void ImportStatement::recurseVisit(NodeVisitor *visitor)
{
    recurseVisitLink(visitor, name);
}

} //namespace KJS
