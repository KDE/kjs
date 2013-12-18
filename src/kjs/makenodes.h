/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2006 Apple Computer, Inc.
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

#ifndef MAKENODES_H
#define MAKENODES_H

#include "nodes.h"
#include "identifier.h"

#define OPTIMIZE_NODES
//#define TRACE_OPTIMIZER

namespace KJS {

// Shorthand wrappers for entering function contexts
static void inFuncExpr() {
    parser().pushFunctionContext(FuncFl_Expr);
}

static void inFuncDecl() {
    parser().pushFunctionContext(FuncFl_Decl);
}


static Node* makeAssignNode(Node* loc, Operator op, Node* expr)
{
    return new AssignNode(loc, op, expr);
}

static Node* makeConditionalNode(Node* l, Node* e1, Node* e2)
{
    return new ConditionalNode(l, e1, e2);
}

static Node* makePrefixNode(Node *expr, Operator op)
{
    return new PrefixNode(expr, op);
}

static Node* makePostfixNode(Node* expr, Operator op)
{
    return new PostfixNode(expr, op);
}

static Node *makeFunctionCallNode(Node *func, ArgumentsNode *args)
{
    Node *n = func->nodeInsideAllParens();

    if (!n->isLocation())
        return new FunctionCallValueNode(func, args);
    else
        return new FunctionCallReferenceNode(func, args);
}

static Node *makeTypeOfNode(Node *expr)
{
    Node *n = expr->nodeInsideAllParens();

    // We only need to use the special path for variable references,
    // since they may throw a ResolveError on evaluate where we don't
    // want that...
    if (n->isVarAccessNode())
        return new TypeOfVarNode(static_cast<VarAccessNode*>(n));
    else
        return new TypeOfValueNode(expr);
}

static Node *makeDeleteNode(Node *expr)
{
    Node *n = expr->nodeInsideAllParens();

    if (!n->isLocation())
        return new DeleteValueNode(expr);
    else
        return new DeleteReferenceNode(static_cast<LocationNode*>(n));//### not 100% faithful listing?
}

static bool makeGetterOrSetterPropertyNode(PropertyNode*& result, Identifier& getOrSet, Identifier& name, ParameterNode *params, FunctionBodyNode *body)
{
    PropertyNode::Type type;

    if (getOrSet == "get")
        type = PropertyNode::Getter;
    else if (getOrSet == "set")
        type = PropertyNode::Setter;
    else
        return false;

    result = new PropertyNode(new PropertyNameNode(name),
                              new FuncExprNode(CommonIdentifiers::shared()->nullIdentifier, body, params), type);

    return true;
}

static Node* makeAddNode(Node* n1, Node* n2, Operator op)
{
#ifdef OPTIMIZE_NODES
    if (n1->isNumber()) {
	if (n2->isNumber()) {
#ifdef TRACE_OPTIMIZER
	    printf("Optimizing ADDNODE NUMBER NUMBER as NUMBER\n");
#endif
	    NumberNode* number1 = static_cast< NumberNode * >(n1);
	    NumberNode* number2 = static_cast< NumberNode * >(n2);
	    double d1 = number1->value();
	    double d2 = number2->value();
	    number1->setValue(op == OpPlus ? d1 + d2 : d1 - d2);
	    return number1;
	}
#ifdef TRACE_OPTIMIZER
	printf("could optimize as ADD NODE NUMBER\n");
#endif
    }
    if (n2->isNumber()) {
#ifdef TRACE_OPTIMIZER
	printf("could optimize as ADD NODE NUMBER\n");
#endif
    }
    if (op == OpPlus && n1->isString() && n2->isString()) {
#ifdef TRACE_OPTIMIZER
	printf("Optimizing ADDNODE STRING STRING as STRING\n");
#endif
	StringNode* str1 = static_cast<StringNode*>(n1);
	StringNode* str2 = static_cast<StringNode*>(n2);
	str1->setValue(str1->value() + str2->value());
	return str1;
    }
#endif
    return new BinaryOperatorNode(n1, n2, op);
}

static Node* makeMultNode(Node* n1, Node* n2, Operator op)
{
#ifdef OPTIMIZE_NODES
    if (n1->isNumber()) {
	if (n2->isNumber()) {
#ifdef TRACE_OPTIMIZER
	    printf("Optimizing MULTNODE NUMBER NUMBER as NUMBER\n");
#endif
	    NumberNode* number1 = static_cast< NumberNode * >(n1);
	    NumberNode* number2 = static_cast< NumberNode * >(n2);
	    double d1 = number1->value();
	    double d2 = number2->value();
	    double res;
	    if (op == OpMult)
		res = d1 * d2;
	    else if (op == OpDiv)
		res = d1 / d2;
	    else // OpMod
		res = fmod(d1, d2);
	    number1->setValue(res);
	    return number1;
	}
#ifdef TRACE_OPTIMIZER
	printf("could optimize as MULT NODE NUMBER\n");
#endif
    }
    if (n2->isNumber()) {
#ifdef TRACE_OPTIMIZER
	printf("could optimize as MULT NODE NUMBER\n");
#endif
    }
#endif
    return new BinaryOperatorNode(n1, n2, op);
}

static Node* makeShiftNode(Node* n1, Node* n2, Operator op)
{
#ifdef OPTIMIZE_NODES
    if (n1->isNumber() && n2->isNumber()) {
#ifdef TRACE_OPTIMIZER
	printf("Optimizing MULTNODE NUMBER NUMBER as NUMBER\n");
#endif
	NumberNode* number1 = static_cast< NumberNode * >(n1);
	NumberNode* number2 = static_cast< NumberNode * >(n2);
	double val = number1->value();
	uint32_t shiftAmount = toUInt32(number2->value());
	switch (op) {
	case OpLShift:
	    // operator <<
	    number1->setValue(toInt32(val) << (shiftAmount & 0x1f));
	    break;
	case OpRShift:
	    // operator >>
	    number1->setValue(toInt32(val) >> (shiftAmount & 0x1f));
	    break;
	case OpURShift:
	    // operator >>>
	    number1->setValue(toUInt32(val) >> (shiftAmount & 0x1f));
	    break;
	default:
	    assert(false);
	    break;
	}
	return number1;
    }
#endif
    return new BinaryOperatorNode(n1, n2, op);
}

static Node* makeRelationalNode(Node* n1, Operator op, Node* n2)
{
    return new BinaryOperatorNode(n1, n2, op);
}

static Node* makeEqualNode(Node* n1, Operator op, Node* n2)
{
    return new BinaryOperatorNode(n1, n2, op);
}

static Node* makeBitOperNode(Node* n1, Operator op, Node* n2)
{
    return new BinaryOperatorNode(n1, n2, op);
}

static Node* makeBinaryLogicalNode(Node* n1, Operator op, Node* n2)
{
    return new BinaryLogicalNode(n1, op, n2);
}

static Node* makeUnaryPlusNode(Node *n)
{
#ifdef OPTIMIZE_NODES
    if (n->isNumber()) {
#ifdef TRACE_OPTIMIZER
    	printf("Optimizing UNARYPLUS NUMBER\n");
#endif
	return n;
    }
#endif
    return new UnaryPlusNode(n);
}

static Node* makeNegateNode(Node *n)
{
#ifdef OPTIMIZE_NODES
    if (n->isNumber()) {
#ifdef TRACE_OPTIMIZER
    	printf("Optimizing NEGATE NUMBER\n");
#endif
	NumberNode *number = static_cast <NumberNode *>(n);
	number->setValue(-number->value());
	return number;
    }
#endif
    return new NegateNode(n);
}

static Node* makeBitwiseNotNode(Node *n)
{
#ifdef OPTIMIZE_NODES
    if (n->isNumber()) {
#ifdef TRACE_OPTIMIZER
    	printf("Optimizing BITWISENOT NUMBER\n");
#endif
	NumberNode *number = static_cast <NumberNode *>(n);
	number->setValue(~toInt32(number->value()));
	return number;
    }
#endif
    return new BitwiseNotNode(n);
}

static Node* makeLogicalNotNode(Node *n)
{
    return new LogicalNotNode(n);
}

static Node* makeGroupNode(Node *n)
{
    if (n->isVarAccessNode() || n->isGroupNode())
	return n;
    return new GroupNode(n);
}

static StatementNode* makeIfNode(Node *e, StatementNode *s1, StatementNode *s2)
{
    return new IfNode(e, s1, s2);
}

static StatementNode *makeImportNode(PackageNameNode *n,
				     bool wildcard, const Identifier &a)
{
    ImportStatement *stat = new ImportStatement(n);
    if (wildcard)
	stat->enableWildcard();
    stat->setAlias(a);

    return stat;
}

static StatementNode *makeLabelNode(const Identifier& l, StatementNode* s)
{
    return new LabelNode(l, s);
}

} // namespace KJS

#endif
// vi: set sw=4 :
