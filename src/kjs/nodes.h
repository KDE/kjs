// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
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

#ifndef NODES_H_
#define NODES_H_

#include "Parser.h"
#include "internal.h"
#include "CompileState.h"
#include "operations.h"
#include "SymbolTable.h"
#include "opcodes.h"
#include "bytecode/opargs.h"
#include <wtf/ListRefPtr.h>
#include <wtf/Vector.h>


namespace KJS {
  class ProgramNode;
  class PropertyNameNode;
  class PropertyListNode;
  class RegExp;
  class SourceElementsNode;
  class SourceStream;
  class PackageObject;
  class FuncDeclNode;
  class FunctionBodyNode;
  class Node;

  class VarDeclVisitor;
  class FuncDeclVisitor;

  class CompileState;
  struct CompileReference;

  class NodeVisitor {
  public:
    virtual ~NodeVisitor() {}
    /**
     This method should be overridden by subclasses to process nodes, and
     perhaps return pointers for replacement nodes. If the node should not be
     changed, return 0. Otherwise, return the replacement node.

     The default implementation asks the node to visit its kids, and do
     replacements on them if needed, but does not change anything for this node
    */
    virtual Node* visit(Node* node);
  };

  class Node {
  public:
      enum NodeType {
          UnknownNodeType,
          NullNodeType,
          BooleanNodeType,
          NumberNodeType,
          StringNodeType,
          RegExpNodeType,
          TryNodeType,
          GroupNodeType,
          LabelNodeType
      };

    Node();
    virtual ~Node();

    virtual NodeType type() const { return UnknownNodeType; }

    UString toString() const;
    
    // This updates line numbers to the pretty-printed version, and 
    // returns it out.
    UString reindent(int baseLine = 0) const;
    
    virtual void streamTo(SourceStream&) const = 0;
    int lineNo() const { return m_line; }

    void ref();
    void deref();
    unsigned refcount();
    static void clearNewNodes();

    virtual Node *nodeInsideAllParens();

    virtual bool isLocation() const       { return false; }
    virtual bool isVarAccessNode() const  { return false; }
    bool isNumber() const { return type() == NumberNodeType; }
    bool isString() const { return type() == StringNodeType; }
    bool isGroupNode() const { return type() == GroupNodeType; }
    bool isTryNode() const { return type() == TryNodeType; }
    bool isLabelNode() const { return type() == LabelNodeType; }
    virtual bool scanForDeclarations () const { return true; }
    virtual bool isIterationStatement()      const { return false; }

    virtual void breakCycle() { }

    // Processes all function and variable declarations below this node,
    // adding them to symbol table or the current object depending on the
    // execution context..
    void processDecls(ExecState*);

    /*
      Implementations of this method should call visitor->visit on all the
      children nodes, and if they return value is non-0, update the link to the child.
      The recurseVisitLink helper takes care of this
    */
    virtual void recurseVisit(NodeVisitor * /*visitor*/) {}

    template<typename T>
    static void recurseVisitLink(NodeVisitor* visitor, RefPtr<T>& link)
    {
        if (!link)
          return;

        T* newLink = static_cast<T*>(visitor->visit(link.get()));
        if (newLink)
          link = newLink;
    }

    template<typename T>
    static void recurseVisitLink(NodeVisitor* visitor, ListRefPtr<T>& link)
    {
        if (!link)
          return;

        T* newLink = static_cast<T*>(visitor->visit(link.get()));
        if (newLink)
          link = newLink;
    }


    JSValue* throwError(ExecState*, ErrorType, const UString& msg);
    JSValue* throwError(ExecState*, ErrorType, const UString& msg, const Identifier&);
    JSValue* throwUndefinedVariableError(ExecState*, const Identifier&);

    virtual OpValue generateEvalCode(CompileState* comp);
  protected:
    mutable int m_line;
  private:
    virtual void processVarDecl (ExecState* state);
    virtual void processFuncDecl(ExecState* state);
    friend class VarDeclVisitor;
    friend class FuncDeclVisitor;

    // disallow assignment
    Node& operator=(const Node&);
    Node(const Node &other);
  };

  class LocationNode : public Node {
  public:
    virtual bool isLocation() const { return true; }

    // For assignments, we need to conceptually evaluate the LHS to a reference before looking at the RHS
    // generateRefBind corresponds to that action. It never issues an error. The returned
    // reference should be passed to generateRefWrite when needed
    virtual CompileReference* generateRefBind(CompileState*) = 0;

    // When we are doing a read-modify-write style op, or just plain read, we want to do a read
    // right after the binding. This does that, and returns a reference for use of follow up
    // writes.
    virtual CompileReference* generateRefRead(CompileState*, OpValue* out) = 0;

    // Writes to a bound reference.
    virtual void generateRefWrite  (CompileState*,
                                    CompileReference* ref, OpValue& valToStore) = 0;

    // The location nodes also handle deletes themselves. Note that this is called
    // w/o generateRefBegin
    virtual OpValue generateRefDelete(CompileState*) = 0;

    // For function calls, we also do a specialized lookup, getting both the valie and the
    // scope/this, also making sure it's not an activation.
    virtual void generateRefFunc(CompileState* comp, OpValue* funOut, OpValue* thisOut) = 0;
  };

  class StatementNode : public Node {
  public:
    StatementNode();
    void setLoc(int line0, int line1) const;
    int firstLine() const { return lineNo(); }
    int lastLine() const { return m_lastLine; }
    void hitStatement(ExecState*);

    void generateDebugInfoIfNeeded(CompileState* comp);

    virtual void generateExecCode(CompileState*);
  private:
    void generateDebugInfo(CompileState* comp);
    mutable int m_lastLine;
  };

  inline void StatementNode::generateDebugInfoIfNeeded(CompileState* comp)
  {
    if (comp->compileType() == Debug)
      generateDebugInfo(comp);
  }

  class NullNode : public Node {
  public:
    NullNode() {}
    virtual NodeType type() const { return NullNodeType; }
    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
  };

  class BooleanNode : public Node {
  public:
    BooleanNode(bool v) : val(v) {}
    bool value() const { return val; }

    virtual NodeType type() const { return BooleanNodeType; }
    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
  private:
    bool val;
  };

  class NumberNode : public Node {
  public:
    NumberNode(double v) : val(v) {}
    double value() const { return val; }
    void setValue(double v) { val = v; }

    virtual NodeType type() const { return NumberNodeType; }
    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
  private:
    double val;
  };

  class StringNode : public Node {
  public:
    StringNode(const UString *v) : val(*v), interned(0) { }
    ~StringNode(); // in nodes2bytecode.cpp
    UString value() const { return val; }
    void setValue(const UString& v) { val = v; }

    virtual NodeType type() const { return StringNodeType; }
    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
  private:
    UString val;
    StringImp* interned;
  };

  class RegExpNode : public Node {
  public:
    RegExpNode(const UString &p, const UString &f)
      : pattern(p), flags(f) { }
    virtual NodeType type() const { return RegExpNodeType; }
    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
  private:
    UString pattern, flags;
  };

  class ThisNode : public Node {
  public:
    ThisNode() {}
    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
  };

  class VarAccessNode : public LocationNode {
  public:
    VarAccessNode(const Identifier& s) : ident(s) {}

    virtual bool isVarAccessNode() const { return true; }
    virtual void streamTo(SourceStream&) const;
    virtual OpValue generateEvalCode(CompileState* comp);

    virtual CompileReference* generateRefBind(CompileState*);
    virtual CompileReference* generateRefRead(CompileState*, OpValue* out);
    virtual void generateRefWrite  (CompileState*,
                                    CompileReference* ref, OpValue& valToStore);
    virtual OpValue generateRefDelete(CompileState*);
    virtual void generateRefFunc(CompileState* comp, OpValue* funOut, OpValue* thisOut);

    // This one never fails..
    OpValue valueForTypeOf(CompileState* comp);

    // Returns the ID this variable should be accessed as, or
    // missingSymbolMarker(), along with the variable's classification
    enum Classification {
        Local,      // local variable accessed by register #
        NonLocal,    // one scope above, unless local injected
        Dynamic,    // need to do a full lookup
        Global      // in the global object, if anywhere.
    };

    size_t classifyVariable(CompileState*, Classification& classify);
  protected:
    Identifier ident;
  };

  class GroupNode : public Node {
  public:
    GroupNode(Node *g) : group(g) { }
    virtual NodeType type() const { return GroupNodeType; }

    virtual OpValue generateEvalCode(CompileState* comp);
    virtual Node *nodeInsideAllParens();
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<Node> group;
  };

  class ElementNode : public Node {
  public:
    // list pointer is tail of a circular list, cracked in the ArrayNode ctor
    ElementNode(int e, Node *n) : next(this), elision(e), node(n) { Parser::noteNodeCycle(this); }
    ElementNode(ElementNode *l, int e, Node *n)
      : next(l->next), elision(e), node(n) { l->next = this; }

    virtual void streamTo(SourceStream&) const;
    PassRefPtr<ElementNode> releaseNext() { return next.release(); }
    virtual void breakCycle();
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    friend class ArrayNode;
    ListRefPtr<ElementNode> next;
    int elision;
    RefPtr<Node> node;
  };

  class ArrayNode : public Node {
  public:
    ArrayNode(int e) : elision(e), opt(true) { }
    ArrayNode(ElementNode *ele)
      : element(ele->next.release()), elision(0), opt(false) { Parser::removeNodeCycle(element.get()); }
    ArrayNode(int eli, ElementNode *ele)
      : element(ele->next.release()), elision(eli), opt(true) { Parser::removeNodeCycle(element.get()); }
    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual bool scanForDeclarations() const { return false; }
  private:
    RefPtr<ElementNode> element;
    int elision;
    bool opt;
  };

  class PropertyNameNode : public Node {
  public:
    PropertyNameNode(const Identifier &s) : str(s) { }
    virtual void streamTo(SourceStream&) const;
  private:
    friend class ObjectLiteralNode;
    Identifier str;
  };

  class PropertyNode : public Node {
  public:
    enum Type { Constant, Getter, Setter };
    PropertyNode(PropertyNameNode *n, Node *a, Type t)
      : name(n), assign(a), type(t) { }
    virtual void streamTo(SourceStream&) const;
    friend class PropertyListNode;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    friend class ObjectLiteralNode;
    RefPtr<PropertyNameNode> name;
    RefPtr<Node> assign;
    Type type;
  };

  class PropertyListNode : public Node {
  public:
    // list pointer is tail of a circular list, cracked in the ObjectLiteralNode ctor
    PropertyListNode(PropertyNode *n)
      : node(n), next(this) { Parser::noteNodeCycle(this); }
    PropertyListNode(PropertyNode *n, PropertyListNode *l)
      : node(n), next(l->next) { l->next = this; }
    virtual void streamTo(SourceStream&) const;
    PassRefPtr<PropertyListNode> releaseNext() { return next.release(); }
    virtual void breakCycle();
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    friend class ObjectLiteralNode;
    RefPtr<PropertyNode> node;
    ListRefPtr<PropertyListNode> next;
  };

  class ObjectLiteralNode : public Node {
  public:
    ObjectLiteralNode() { }
    ObjectLiteralNode(PropertyListNode *l) : list(l->next.release()) { Parser::removeNodeCycle(list.get()); }
    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual bool scanForDeclarations() const { return false; }
  private:
    RefPtr<PropertyListNode> list;
  };

  class BracketAccessorNode : public LocationNode {
  public:
    BracketAccessorNode(Node *e1, Node *e2) : expr1(e1), expr2(e2) {}
    virtual void streamTo(SourceStream&) const;

    virtual OpValue generateEvalCode(CompileState* comp);

    virtual CompileReference* generateRefBind(CompileState*);
    virtual CompileReference* generateRefRead(CompileState*, OpValue* out);
    virtual void generateRefWrite  (CompileState*,
                                    CompileReference* ref, OpValue& valToStore);
    virtual OpValue generateRefDelete(CompileState*);
    virtual void generateRefFunc(CompileState* comp, OpValue* funOut, OpValue* thisOut);

    Node *base() { return expr1.get(); }
    Node *subscript() { return expr2.get(); }

    virtual void recurseVisit(NodeVisitor *visitor);
  protected:
    RefPtr<Node> expr1;
    RefPtr<Node> expr2;
  };

  class DotAccessorNode : public LocationNode {
  public:
    DotAccessorNode(Node *e, const Identifier &s) : expr(e), ident(s) { }
    virtual void streamTo(SourceStream&) const;

    virtual OpValue generateEvalCode(CompileState* comp);

    virtual CompileReference* generateRefBind(CompileState*);
    virtual CompileReference* generateRefRead(CompileState*, OpValue* out);
    virtual void generateRefWrite  (CompileState*,
                                    CompileReference* ref, OpValue& valToStore);
    virtual OpValue generateRefDelete(CompileState*);
    virtual void generateRefFunc(CompileState* comp, OpValue* funOut, OpValue* thisOut);

    Node *base() const { return expr.get(); }
    const Identifier& identifier() const { return ident; }

    virtual void recurseVisit(NodeVisitor *visitor);
  protected:
    RefPtr<Node> expr;
    Identifier ident;
  };

  class ArgumentListNode : public Node {
  public:
    // list pointer is tail of a circular list, cracked in the ArgumentsNode ctor
    ArgumentListNode(Node *e) : next(this), expr(e) { Parser::noteNodeCycle(this); }
    ArgumentListNode(ArgumentListNode *l, Node *e)
      : next(l->next), expr(e) { l->next = this; }

    virtual void streamTo(SourceStream&) const;
    PassRefPtr<ArgumentListNode> releaseNext() { return next.release(); }
    virtual void breakCycle();

    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    friend class ArgumentsNode;
    ListRefPtr<ArgumentListNode> next;
    RefPtr<Node> expr;
  };

  class ArgumentsNode : public Node {
  public:
    ArgumentsNode() { }
    ArgumentsNode(ArgumentListNode *l)
      : list(l->next.release()) { Parser::removeNodeCycle(list.get()); }

    void generateEvalArguments(CompileState* comp);
    virtual void streamTo(SourceStream&) const;

    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<ArgumentListNode> list;
  };

  class NewExprNode : public Node {
  public:
    NewExprNode(Node *e) : expr(e) {}
    NewExprNode(Node *e, ArgumentsNode *a) : expr(e), args(a) {}

    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<Node> expr;
    RefPtr<ArgumentsNode> args;
  };

  class FunctionCallValueNode : public Node {
  public:
    FunctionCallValueNode(Node *e, ArgumentsNode *a) : expr(e), args(a) {}

    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<Node> expr;
    RefPtr<ArgumentsNode> args;
  };

  class FunctionCallReferenceNode : public Node {
  public:
    FunctionCallReferenceNode(Node *e, ArgumentsNode *a) : expr(e), args(a) {}

    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<Node> expr;
    RefPtr<ArgumentsNode> args;
  };

  class PostfixNode : public Node {
  public:
    PostfixNode(Node *l, Operator o) : m_loc(l), m_oper(o) {}

    void streamTo(SourceStream&) const;
    void recurseVisit(NodeVisitor * visitor);
    virtual OpValue generateEvalCode(CompileState* comp);
  protected:
    RefPtr<Node> m_loc;
    Operator m_oper;
  };

  class DeleteReferenceNode : public Node {
  public:
    DeleteReferenceNode(LocationNode *l) : loc(l) {}

    void streamTo(SourceStream&) const;
    void recurseVisit(NodeVisitor * visitor);
    virtual OpValue generateEvalCode(CompileState* comp);
  private:
    RefPtr<LocationNode> loc;
  };

  class DeleteValueNode : public Node {
  public:
    DeleteValueNode(Node *e) : m_expr(e) {}

    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual OpValue generateEvalCode(CompileState* comp);
  private:
    RefPtr<Node> m_expr;
  };


  class VoidNode : public Node {
  public:
    VoidNode(Node *e) : expr(e) {}

    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<Node> expr;
  };

  class TypeOfVarNode : public Node {
  public:
    TypeOfVarNode(VarAccessNode *l) : loc(l) {}

    virtual OpValue generateEvalCode(CompileState* comp);
    void streamTo(SourceStream&) const;
    void recurseVisit(NodeVisitor * visitor);
  private:
    RefPtr<VarAccessNode> loc;
  };

  class TypeOfValueNode : public Node {
  public:
    TypeOfValueNode(Node *e) : m_expr(e) {}

    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<Node> m_expr;
  };

  class PrefixNode : public Node {
  public:
    PrefixNode(Node *l, Operator o) : m_loc(l), m_oper(o) {}

    virtual OpValue generateEvalCode(CompileState* comp);
    void streamTo(SourceStream&) const;
    void recurseVisit(NodeVisitor * visitor);
  protected:
    RefPtr<Node> m_loc;
    Operator m_oper;
  };

  class UnaryPlusNode : public Node {
  public:
    UnaryPlusNode(Node *e) : expr(e) {}

    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<Node> expr;
  };

  class NegateNode : public Node {
  public:
    NegateNode(Node *e) : expr(e) {}

    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<Node> expr;
  };

  class BitwiseNotNode : public Node {
  public:
    BitwiseNotNode(Node *e) : expr(e) {}

    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<Node> expr;
  };

  class LogicalNotNode : public Node {
  public:
    LogicalNotNode(Node *e) : expr(e) {}

    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<Node> expr;
  };

  class BinaryOperatorNode : public Node {
  public:
    BinaryOperatorNode(Node* e1, Node* e2, Operator op)
      : expr1(e1), expr2(e2), oper(op) {}

    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor* visitor);
  private:
    RefPtr<Node> expr1;
    RefPtr<Node> expr2;
    Operator oper;
  };

  /**
   * expr1 && expr2, expr1 || expr2
   */
  class BinaryLogicalNode : public Node {
  public:
    BinaryLogicalNode(Node *e1, Operator o, Node *e2) :
      expr1(e1), expr2(e2), oper(o) {}

    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<Node> expr1;
    RefPtr<Node> expr2;
    Operator oper;
  };

  /**
   * The ternary operator, "logical ? expr1 : expr2"
   */
  class ConditionalNode : public Node {
  public:
    ConditionalNode(Node *l, Node *e1, Node *e2) :
      logical(l), expr1(e1), expr2(e2) {}

    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<Node> logical;
    RefPtr<Node> expr1;
    RefPtr<Node> expr2;
  };

  class AssignNode : public Node {
  public:
    AssignNode(Node* loc, Operator oper, Node *right)
      : m_loc(loc), m_oper(oper), m_right(right) {}

    void streamTo(SourceStream&) const;
    virtual OpValue generateEvalCode(CompileState* comp);
    void recurseVisit(NodeVisitor * visitor);
  protected:
    RefPtr<Node> m_loc;
    Operator m_oper;
    RefPtr<Node> m_right;
  };

  class CommaNode : public Node {
  public:
    CommaNode(Node *e1, Node *e2) : expr1(e1), expr2(e2) {}

    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual OpValue generateEvalCode(CompileState* comp);
  private:
    RefPtr<Node> expr1;
    RefPtr<Node> expr2;
  };

  class AssignExprNode : public Node {
  public:
    AssignExprNode(Node *e) : expr(e) {}

    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual OpValue generateEvalCode(CompileState* comp);

    Node* getExpr() { return expr.get(); }
  private:
    RefPtr<Node> expr;
  };

  class VarDeclNode : public Node {
  public:
    enum Type { Variable, Constant };
    VarDeclNode(const Identifier &id, AssignExprNode *in, Type t);

    void generateCode(CompileState* comp);

    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);

    virtual void processVarDecl(ExecState*);
  private:
    friend class VarStatementNode;
    friend class VarDeclListNode;
    Type varType;
    Identifier ident;
    RefPtr<AssignExprNode> init;
  };

  class VarDeclListNode : public Node {
  public:
    // list pointer is tail of a circular list, cracked in the ForNode/VarStatementNode ctor
    VarDeclListNode(VarDeclNode *v) : next(this), var(v) { Parser::noteNodeCycle(this); }
    VarDeclListNode(VarDeclListNode *l, VarDeclNode *v)
      : next(l->next), var(v) { l->next = this; }

    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    PassRefPtr<VarDeclListNode> releaseNext() { return next.release(); }
    virtual void breakCycle();
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    friend class ForNode;
    friend class VarStatementNode;
    ListRefPtr<VarDeclListNode> next;
    RefPtr<VarDeclNode> var;
  };

  class VarStatementNode : public StatementNode {
  public:
    VarStatementNode(VarDeclListNode *l) : next(l->next.release()) { Parser::removeNodeCycle(next.get()); }

    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual void generateExecCode(CompileState*);
  private:
    RefPtr<VarDeclListNode> next;
  };

  class BlockNode : public StatementNode {
  public:
    BlockNode(SourceElementsNode *s);

    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual void generateExecCode(CompileState*);
  protected:
    RefPtr<SourceElementsNode> source;
  };

  class EmptyStatementNode : public StatementNode {
  public:
    EmptyStatementNode() { } // debug

    virtual void streamTo(SourceStream&) const;
    virtual void generateExecCode(CompileState*);
  };

  class ExprStatementNode : public StatementNode {
  public:
    ExprStatementNode(Node *e) : expr(e) { }

    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual void generateExecCode(CompileState*);
  private:
    RefPtr<Node> expr;
  };

  class IfNode : public StatementNode {
  public:
    IfNode(Node *e, StatementNode *s1, StatementNode *s2)
      : expr(e), statement1(s1), statement2(s2) {}

    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual void generateExecCode(CompileState*);
  private:
    RefPtr<Node> expr;
    RefPtr<StatementNode> statement1;
    RefPtr<StatementNode> statement2;
  };

  class DoWhileNode : public StatementNode {
  public:
    DoWhileNode(StatementNode *s, Node *e) : statement(s), expr(e) {}

    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual void generateExecCode(CompileState*);
    virtual bool isIterationStatement() const { return true; }
  private:
    RefPtr<StatementNode> statement;
    RefPtr<Node> expr;
  };

  class WhileNode : public StatementNode {
  public:
    WhileNode(Node *e, StatementNode *s) : expr(e), statement(s) {}

    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual void generateExecCode(CompileState*);
    virtual bool isIterationStatement() const { return true; }
  private:
    RefPtr<Node> expr;
    RefPtr<StatementNode> statement;
  };

  class ForNode : public StatementNode {
  public:
    ForNode(Node *e1, Node *e2, Node *e3, StatementNode *s) :
      expr1(e1), expr2(e2), expr3(e3), statement(s) {}
    ForNode(VarDeclListNode *e1, Node *e2, Node *e3, StatementNode *s) :
      expr1(e1->next.release()), expr2(e2), expr3(e3), statement(s) { Parser::removeNodeCycle(expr1.get()); }

    virtual void generateExecCode(CompileState*);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual bool isIterationStatement() const { return true; }
  private:
    RefPtr<Node> expr1;
    RefPtr<Node> expr2;
    RefPtr<Node> expr3;
    RefPtr<StatementNode> statement;
  };

  class ForInNode : public StatementNode {
  public:
    ForInNode(Node *l, Node *e, StatementNode *s);
    ForInNode(const Identifier &i, AssignExprNode *in, Node *e, StatementNode *s);

    virtual void generateExecCode(CompileState*);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual bool isIterationStatement() const { return true; }
  private:
    Identifier ident;
    RefPtr<AssignExprNode> init;
    RefPtr<Node> lexpr;
    RefPtr<Node> expr;
    RefPtr<VarDeclNode> varDecl;
    RefPtr<StatementNode> statement;
  };

  class ContinueNode : public StatementNode {
  public:
    ContinueNode() : target(0) { }
    ContinueNode(const Identifier &i) : ident(i), target(0) { }

    virtual void generateExecCode(CompileState*);
    virtual void streamTo(SourceStream&) const;
  private:
    Identifier  ident;
    const Node* target;
  };

  class BreakNode : public StatementNode {
  public:
    BreakNode() : target(0) { }
    BreakNode(const Identifier &i) : ident(i), target(0) { }

    virtual void generateExecCode(CompileState*);
    virtual void streamTo(SourceStream&) const;
  private:
    Identifier ident;
    const Node* target;
  };

  class ReturnNode : public StatementNode {
  public:
    ReturnNode(Node *v) : value(v) {}

    virtual void generateExecCode(CompileState*);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<Node> value;
  };

  class WithNode : public StatementNode {
  public:
    WithNode(Node *e, StatementNode *s) : expr(e), statement(s) {}

    virtual void generateExecCode(CompileState*);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<Node> expr;
    RefPtr<StatementNode> statement;
  };

  class LabelNode : public StatementNode {
  public:
    LabelNode(const Identifier &l, StatementNode *s) : label(l), statement(s) { }

    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual void generateExecCode(CompileState*);
    virtual NodeType type() const { return LabelNodeType; }
  private:
    Identifier label;
    RefPtr<StatementNode> statement;
  };

  class ThrowNode : public StatementNode {
  public:
    ThrowNode(Node *e) : expr(e) {}

    virtual void generateExecCode(CompileState*);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<Node> expr;
  };

  class TryNode : public StatementNode {
  public:
    TryNode(StatementNode *b, const Identifier &e, StatementNode *c, StatementNode *f)
      : tryBlock(b), exceptionIdent(e), catchBlock(c), finallyBlock(f) { }
    virtual NodeType type() const { return TryNodeType; }

    virtual void generateExecCode(CompileState*);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    RefPtr<StatementNode> tryBlock;
    Identifier exceptionIdent;
    RefPtr<StatementNode> catchBlock;
    RefPtr<StatementNode> finallyBlock;
  };

  class ParameterNode : public Node {
  public:
    // list pointer is tail of a circular list, cracked in the FuncDeclNode/FuncExprNode ctor
    ParameterNode(const Identifier &i) : id(i), next(this) { Parser::noteNodeCycle(this); }
    ParameterNode(ParameterNode *next, const Identifier &i)
      : id(i), next(next->next) { next->next = this; }

    const Identifier& ident() const { return id; }
    ParameterNode *nextParam() const { return next.get(); }
    virtual void streamTo(SourceStream&) const;
    PassRefPtr<ParameterNode> releaseNext() { return next.release(); }
    virtual void breakCycle();

    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    friend class FuncDeclNode;
    friend class FuncExprNode;
    Identifier id;
    ListRefPtr<ParameterNode> next;
  };

  // Flags about function bodies we care about for codegen
  enum FunctionBodyFlags {
    // note: neither of the two below is set for things created via
    // top-level, eval, or function ctor
    FuncFl_Decl      = 1,
    FuncFl_Expr      = 2, 
    FuncFl_HasEvalOp = 4
  };

  /**
   This AST node corresponds to the function body or top-level code in the AST, but is used to
   keep track of much of the information relevant to the whole function,
   such as parameter names and symbol tables. This is because there are both function
   declarations and expressions, so there is no natural single place to put this stuff
   above the body

   inherited by ProgramNode
  */
  class FunctionBodyNode : public BlockNode {
  public:
    struct SymbolInfo {
      SymbolInfo(int _attr, FuncDeclNode* _funcDecl) : funcDecl(_funcDecl), attr(_attr) {}
      SymbolInfo() {}
      FuncDeclNode* funcDecl;
      int           attr;
    };
    FunctionBodyNode(SourceElementsNode *);
    int sourceId() { return m_sourceId; }
    const UString& sourceURL() { return m_sourceURL; }

    bool isCompiled() const { return m_compType != NotCompiled; }
    void compileIfNeeded(CodeType ctype, CompileType compType);
    void compile(CodeType ctype, CompileType compType);
    CompileType compileState() const { return m_compType; }

    virtual void generateExecCode(CompileState*);

    // Reserves a register for private use, making sure that id is in the right spot..
    void reserveSlot(size_t id, bool shouldMark);

    // Symbol table functions
    SymbolTable& symbolTable() { return m_symbolTable; }
    size_t lookupSymbolID(const Identifier& id) const { return m_symbolTable.get(id.ustring().rep()); }

    int  numLocalsAndRegisters() const { return m_symbolList.size(); }
    SymbolInfo* getLocalInfo()         { return m_symbolList.data(); }

    size_t  numFunctionLocals() const { return m_functionLocals.size(); }
    size_t* getFunctionLocalInfo()    { return m_functionLocals.data(); }

    // Parameter stuff. We only collect the names during the parsing/
    // while FunctionImp is responsible for managing the IDs.
    void addParam(const Identifier& ident);
    size_t numParams() const { return m_paramList.size(); }
    const Identifier& paramName(size_t pos) const { return m_paramList[pos]; }

    void addVarDecl(const Identifier& ident, int attr, ExecState* exec);
    void addFunDecl(const Identifier& ident, int attr, FuncDeclNode* funcDecl);

    // Adds a new symbol, killing any previous ID.
    void addSymbolOverwriteID(size_t id, const Identifier& ident, int attr);

    // Runs the code, compiling if needed. This should only be used for non-function ExecStates
    Completion execute(ExecState *exec);

    bool tearOffAtEnd() const { return m_tearOffAtEnd; }

    const CodeBlock& code() const { return m_compiledCode; }
    CodeBlock& code() { return m_compiledCode; }

    // Collection of FuncFl_* flags describing information collected about this function
    // during the parsing.
    unsigned flags() const { return m_flags; }

  private:
    size_t addSymbol(const Identifier& ident, int attr, FuncDeclNode* funcDecl = 0);
    UString m_sourceURL;
    int m_sourceId : 31;
    bool m_tearOffAtEnd : 1;        
    CompileType m_compType;

    // Flags 
    unsigned m_flags;

    // This maps id -> attributes and function decl info
    WTF::Vector<SymbolInfo> m_symbolList;

    // This contains the list of locals which contains function declarations
    WTF::Vector<size_t> m_functionLocals;

    // This maps name -> id
    SymbolTable m_symbolTable;

    // The list of parameter names
    WTF::Vector<Identifier> m_paramList;

    CodeBlock m_compiledCode;
  };

  inline void FunctionBodyNode::compileIfNeeded(CodeType ctype, CompileType compType) {
    if (m_compType != compType)
        compile(ctype, compType);
  }

  class FuncExprNode : public Node {
  public:
    FuncExprNode(const Identifier &i, FunctionBodyNode *b, ParameterNode *p = 0)
      : ident(i), param(p ? p->next.release() : PassRefPtr<ParameterNode>(0)), body(b) { if (p) { Parser::removeNodeCycle(param.get()); } addParams(); }

    virtual OpValue generateEvalCode(CompileState* comp);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual bool scanForDeclarations() const { return false; }
  private:
    void addParams();
    // Used for streamTo
    friend class PropertyNode;
    Identifier ident;
    RefPtr<ParameterNode> param;
    RefPtr<FunctionBodyNode> body;
  };

  class FuncDeclNode : public StatementNode {
  public:
    FuncDeclNode(const Identifier &i, FunctionBodyNode *b)
      : ident(i), body(b) { addParams(); }
    FuncDeclNode(const Identifier &i, ParameterNode *p, FunctionBodyNode *b)
      : ident(i), param(p->next.release()), body(b) { Parser::removeNodeCycle(param.get()); addParams(); }

    virtual void generateExecCode(CompileState*);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
    virtual bool scanForDeclarations() const { return false; }

    virtual void processFuncDecl(ExecState*);
    FunctionImp* makeFunctionObject(ExecState*);
  private:
    void addParams();
    Identifier ident;
    RefPtr<ParameterNode> param;
    RefPtr<FunctionBodyNode> body;
  };

  // A linked list of source element nodes
  class SourceElementsNode : public StatementNode {
  public:
    // list pointer is tail of a circular list, cracked in the BlockNode (or subclass) ctor
    SourceElementsNode(StatementNode*);
    SourceElementsNode(SourceElementsNode *s1, StatementNode *s2);

    virtual void generateExecCode(CompileState*);
    virtual void streamTo(SourceStream&) const;
    PassRefPtr<SourceElementsNode> releaseNext() { return next.release(); }
    virtual void breakCycle();
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    friend class BlockNode;
    friend class CaseClauseNode;
    RefPtr<StatementNode> node;
    ListRefPtr<SourceElementsNode> next;
  };

  class CaseClauseNode : public Node {
  public:
      CaseClauseNode(Node *e) : expr(e) { }
      CaseClauseNode(Node *e, SourceElementsNode *s)
      : expr(e), source(s->next.release()) { Parser::removeNodeCycle(source.get()); }

      virtual void streamTo(SourceStream&) const;
      virtual void recurseVisit(NodeVisitor *visitor);
  private:
      friend class SwitchNode;
      RefPtr<Node> expr;
      RefPtr<SourceElementsNode> source;
  };

  class ClauseListNode : public Node {
  public:
      // list pointer is tail of a circular list, cracked in the CaseBlockNode ctor
      ClauseListNode(CaseClauseNode *c) : clause(c), next(this) { Parser::noteNodeCycle(this); }
      ClauseListNode(ClauseListNode *n, CaseClauseNode *c)
      : clause(c), next(n->next) { n->next = this; }

      CaseClauseNode *getClause() const { return clause.get(); }
      ClauseListNode *getNext() const { return next.get(); }
      virtual void streamTo(SourceStream&) const;
      PassRefPtr<ClauseListNode> releaseNext() { return next.release(); }
      virtual void breakCycle();
      virtual void recurseVisit(NodeVisitor *visitor);
  private:
      friend class SwitchNode;
      friend class CaseBlockNode;
      RefPtr<CaseClauseNode> clause;
      ListRefPtr<ClauseListNode> next;
  };

  class CaseBlockNode : public Node {
  public:
      CaseBlockNode(ClauseListNode *l1, CaseClauseNode *d, ClauseListNode *l2);

      virtual void streamTo(SourceStream&) const;
      virtual void recurseVisit(NodeVisitor *visitor);
  private:
      friend class SwitchNode;
      RefPtr<ClauseListNode> list1;
      RefPtr<CaseClauseNode> def;
      RefPtr<ClauseListNode> list2;
  };

  class SwitchNode : public StatementNode {
  public:
      SwitchNode(Node *e, CaseBlockNode *b) : expr(e), block(b) { }

      virtual void streamTo(SourceStream&) const;
      virtual void recurseVisit(NodeVisitor *visitor);
      virtual void generateExecCode(CompileState* comp);
  private:
      RefPtr<Node> expr;
      RefPtr<CaseBlockNode> block;
  };

  // important: these are also built when compiling things via the Function constructor
  // (see FunctionObjectImp::construct() and Parser::parseFunctionBody, so the existence
  // of this class rather than the bare FunctionBodyNode does not care much information.
  class ProgramNode : public FunctionBodyNode {
  public:
    ProgramNode(SourceElementsNode *s);
    virtual void streamTo(SourceStream&) const;
  };

  class PackageNameNode : public Node {
  public:
    PackageNameNode(const Identifier &i) : names(0), id(i) { }
    PackageNameNode(PackageNameNode *n,
                    const Identifier &i) : names(n), id(i) { }

    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);

    Completion loadSymbol(ExecState* exec, bool wildcard);
    PackageObject* resolvePackage(ExecState* exec);

  private:
    PackageObject* resolvePackage(ExecState* exec,
				  JSObject* baseObject, Package* basePackage);
    RefPtr<PackageNameNode> names;
    Identifier id;
  };

  class ImportStatement : public StatementNode {
  public:
    ImportStatement(PackageNameNode *n) : name(n), wld(false) {}
    void enableWildcard() { wld = true; }
    void setAlias(const Identifier &a) { al = a; }

    virtual void generateExecCode(CompileState*);
    virtual void streamTo(SourceStream&) const;
    virtual void recurseVisit(NodeVisitor *visitor);
  private:
    virtual void processVarDecl (ExecState* state);
    RefPtr<PackageNameNode> name;
    Identifier al;
    bool wld;
  };

} // namespace

#endif
// kate: indent-width 2; replace-tabs on; tab-width 4; space-indent on;
