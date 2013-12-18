/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2002 Harri Porten (porten@kde.org)
 *  Copyright (C) 2003, 2004, 2005, 2006, 2007 Apple Inc. All rights reserved.
 *  Copyright (C) 2003 Apple Computer, Inc.
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

#include <ctype.h>
#include <stdio.h>

#include "nodes.h"
#include "function.h"
#include "scriptfunction.h"

#include <typeinfo>

#define NOINLINE
#if COMPILER(CWP)
#pragma auto_inline off
#elif COMPILER(MSVC)
#pragma auto_inline(off)
#elif COMPILER(GCC)
// #undef  NOINLINE
// #define NOINLINE    __attribute__ (noinline)
#endif

// GCC cstring uses these automatically, but not all implementations do.
using std::strlen;
using std::strcpy;
using std::strncpy;
using std::memset;
using std::memcpy;

namespace KJS
{
const bool kDontQuote = false, kQuote = true;

/**
 * A simple text streaming class that helps with code indentation.
 */
class SourceStream
{
public:
    enum eEndl     { Endl };
    enum eIndent   { Indent };
    enum eUnindent { Unindent };

    static const int kBufSize = 2048;
    typedef unsigned short UTF16;
    SourceStream() : indent(0), bufUsed(0), reindentLine(0), reindenting(false) {}
    const UString &toString()
    {
        flush();
        return str;
    }
    SourceStream &operator<<(const Identifier &s) NOINLINE;
    SourceStream &operator<<(const UString &s) NOINLINE;
    SourceStream &operator<<(const char *s) NOINLINE;
    SourceStream &operator<<(char s) NOINLINE;
    SourceStream &operator<<(eEndl) NOINLINE;
    SourceStream &operator<<(const Node *n) NOINLINE;
    SourceStream &operator<<(const StatementNode *n) NOINLINE;
    SourceStream &operator<<(Operator op) NOINLINE;
    inline SourceStream &operator<<(eIndent)
    {
        indent += 2;
        return *this;
    }
    inline SourceStream &operator<<(eUnindent)
    {
        indent -= 2;
        return *this;
    }
    SourceStream &append(const Node *expr1, const char *sep, const Node *expr2) NOINLINE;
    SourceStream &append(const RefPtr<Node> &expr1, const char *sep, const RefPtr<Node> &expr2) NOINLINE;
    SourceStream &append(const UTF16 *src, int srcLen) NOINLINE;
    SourceStream &append(const UString &inStr, bool quote) NOINLINE;
    template <typename T>
    inline SourceStream &operator<<(const RefPtr<T> &n)
    {
        return this->operator<<(n.get());
    }

    void setReindenting(int baseLine)
    {
        reindenting = true;
        reindentLine = baseLine;
    }
private:
    UString str;
    int indent;
    int bufUsed;
    UTF16 buffer[kBufSize];
    void flush() NOINLINE;
    void put(UTF16 ch)
    {
        buffer[bufUsed++] = ch;
    }
    void put(char ch)
    {
        buffer[bufUsed++] = static_cast<unsigned char>(ch);
    }

    int  reindentLine;
    bool reindenting;
};
}

using namespace KJS;

SourceStream &SourceStream::operator<<(Operator op)
{
    assert(op == OpPlusPlus || op == OpMinusMinus);
    return *this << ((op == OpPlusPlus) ? "++" : "--");
}

void SourceStream::flush()
{
    if (bufUsed) {
        str.append(UString(reinterpret_cast<const UChar *>(buffer), bufUsed));
        bufUsed = 0;
    }
}

SourceStream &SourceStream::operator<<(char c)
{
    if (bufUsed == kBufSize) {
        flush();
    }
    put(c);
    return *this;
}

SourceStream &SourceStream::operator<<(const char *s)
{
    assert(strlen(s) < 100);
    if (bufUsed > kBufSize - 100) {
        flush();
    }

    unsigned char ch;
    int i = bufUsed;
    --s;
    for (UTF16 *dst = &buffer[i] - 1; (ch = *++s) != 0; ++i) {
        *++dst = ch;
    }
    bufUsed = i;

    return *this;
}

SourceStream &SourceStream::operator<<(const UString &s)
{
    return append(&s.data()->uc, s.size());
}

SourceStream &SourceStream::operator<<(const Identifier &s)
{
    return append(s.ustring(), kDontQuote);
}

SourceStream &SourceStream::operator<<(const StatementNode *n)
{
    if (n) {
        // Update debug info with new line numbers if needed.
        // Note that streamTo will output endLine first thing,
        // so we want the next line in the debug info
        int firstLine = reindentLine + 1;
        n->streamTo(*this);
        if (reindenting) {
            n->setLoc(firstLine, reindentLine - 1);
        }
    }
    return *this;
}

SourceStream &SourceStream::operator<<(const Node *n)
{
    if (n) {
        n->streamTo(*this);
    }
    return *this;
}

SourceStream &SourceStream::operator<<(eEndl)
{
    if (bufUsed > kBufSize - 1 - indent) {
        flush();
    }
    put('\n');
    ++reindentLine;

    if (indent > 0) {
        UTF16 *dst = &buffer[bufUsed];
        for (int i = indent; i > 0; --i) {
            *dst++ = ' ';
        }
        bufUsed += indent;
    }

    return *this;
}

SourceStream &SourceStream::append(const Node *expr1, const char *sep, const Node *expr2)
{
    return *this << expr1 << sep << expr2;
}

SourceStream &
SourceStream::append(const RefPtr<Node> &expr1, const char *sep, const RefPtr<Node> &expr2)
{
    return *this << expr1 << sep << expr2;
}

SourceStream &SourceStream::append(const UTF16 *src, int srcLen)
{
    if (kBufSize - bufUsed < srcLen) {
        flush();
    }
    if (kBufSize - bufUsed < srcLen) {
        str.append(UString(reinterpret_cast<const UChar *>(src), srcLen));
    } else {
        UTF16 *dst = &buffer[bufUsed];
        bufUsed += srcLen;
//    while (--srcLen >= 0)
        while (srcLen-- > 0) {
            *dst++ = *src++;
        }
    }

    return *this;
}

// Append a quoted string
SourceStream &SourceStream::append(const UString &inStr, bool quote)
{
    if (quote) {
        *this << '"';
    }
    const UTF16 *src = &inStr.data()->uc;
    const size_t size = inStr.size();
    for (size_t i = 0; i < size; ++i) {
        if (bufUsed >= kBufSize - 8) {
            flush();
        }
        UTF16 c = *src++, esc = '\\';
        switch (c) {
        case '\"':                break;
        case '\n':    c = 'n';    break;
        case '\r':    c = 'r';    break;
        case '\t':    c = 't';    break;
        case '\\':                break;
        default:
            if (c >= 128 || !isprint(c)) { // ### FIXME: use Unicode tables
                char hexValue[8];
                int len = sprintf(hexValue, (c < 256) ? "\\x%02X" : "\\u%04X", c);
                UTF16 *dst = &buffer[bufUsed];
                bufUsed += len;
                for (int j = 0; j < len; ++j) {
                    dst[j] = hexValue[j];
                }
                continue;
            }
            esc = 0;    // don't escape
            break;
        }
        if (esc) {
            put(esc);
        }
        put(c);
    }

    if (quote) {
        *this << '"';
    }
    return *this;
}

UString FunctionImp::toSource() const
{
    SourceStream str;
    str << "function ";
    str.append(functionName().ustring(), kDontQuote) << '(';
    const FunctionBodyNode *body = this->body.get();
    const int numParams = body->numParams();
    for (int i = 0; i < numParams; ++i) {
        if (i > 0) {
            str << ", ";
        }
        str << body->paramName(i).ustring();
    }
    str << ") ";
    body->streamTo(str);

    return str.toString();
}

UString Node::toString() const
{
    SourceStream str;
    streamTo(str);

    return str.toString();
}

UString Node::reindent(int baseLine) const
{
    SourceStream str;
    str.setReindenting(baseLine);

    streamTo(str);

    return str.toString();
}

void NullNode::streamTo(SourceStream &s) const
{
    s << "null";
}

void BooleanNode::streamTo(SourceStream &s) const
{
    s << (value() ? "true" : "false");
}

void NumberNode::streamTo(SourceStream &s) const
{
    s << UString::from(value());
}

void StringNode::streamTo(SourceStream &s) const
{
    s.append(value(), kQuote);
}

void RegExpNode::streamTo(SourceStream &s) const
{
    s <<  '/' << pattern << '/' << flags;
}

void ThisNode::streamTo(SourceStream &s) const
{
    s << "this";
}

void VarAccessNode::streamTo(SourceStream &s) const
{
    s << ident;
}

void GroupNode::streamTo(SourceStream &s) const
{
    s << '(' << group << ')';
}

void ElementNode::streamTo(SourceStream &s) const
{
    for (const ElementNode *n = this; n; n = n->next.get()) {
        for (int i = 0; i < n->elision; i++) {
            s << ',';
        }
        s << n->node;
        if (n->next) {
            s << ',';
        }
    }
}

void ArrayNode::streamTo(SourceStream &s) const
{
    s << '[' << element;
    for (int i = 0; i < elision; i++) {
        s << ',';
    }
    // Parser consumes one elision comma if there's array elements
    // present in the expression.
    if (opt && element) {
        s << ',';
    }
    s << ']';
}

void ObjectLiteralNode::streamTo(SourceStream &s) const
{
    if (list) {
        s << "{ " << list << " }";
    } else {
        s << "{ }";
    }
}

void PropertyListNode::streamTo(SourceStream &s) const
{
    s << node;

    for (const PropertyListNode *n = next.get(); n; n = n->next.get()) {
        s << ", " << n->node;
    }
}

void PropertyNode::streamTo(SourceStream &s) const
{
    switch (type) {
    case Constant:
        s << name << ": " << assign;
        break;
    case Getter:
    case Setter: {
        const FuncExprNode *func = static_cast<const FuncExprNode *>(assign.get());
        if (type == Getter) {
            s << "get ";
        } else {
            s << "set ";
        }

        s << name << '(' << func->param << ')' << func->body;
        break;
    }
    }
}

void PropertyNameNode::streamTo(SourceStream &s) const
{
    s.append(str.ustring(), kQuote);
}

void BracketAccessorNode::streamTo(SourceStream &s) const
{
    s.append(expr1, "[", expr2) << ']';
}

void DotAccessorNode::streamTo(SourceStream &s) const
{
    s << expr << '.' << ident;
}

void ArgumentListNode::streamTo(SourceStream &s) const
{
    s << expr;
    for (ArgumentListNode *n = next.get(); n; n = n->next.get()) {
        s << ", " << n->expr;
    }
}

void ArgumentsNode::streamTo(SourceStream &s) const
{
    s << '(' << list << ')';
}

void NewExprNode::streamTo(SourceStream &s) const
{
    s << "new " << expr << args;
}

void FunctionCallValueNode::streamTo(SourceStream &s) const
{
    s << expr << args;
}

void FunctionCallReferenceNode::streamTo(SourceStream &s) const
{
    s << expr << args;
}

void PostfixNode::streamTo(SourceStream &s) const
{
    s << m_loc << m_oper;
}

void DeleteReferenceNode::streamTo(SourceStream &s) const
{
    s << "delete " << loc;
}

void DeleteValueNode::streamTo(SourceStream &s) const
{
    s << "delete " << m_expr;
}

void VoidNode::streamTo(SourceStream &s) const
{
    s << "void " << expr;
}

void TypeOfValueNode::streamTo(SourceStream &s) const
{
    s << "typeof " << m_expr;
}

void TypeOfVarNode::streamTo(SourceStream &s) const
{
    s << "typeof " << loc;
}

void PrefixNode::streamTo(SourceStream &s) const
{
    s << m_oper << m_loc;
}

void UnaryPlusNode::streamTo(SourceStream &s) const
{
    s << "+ " << expr;
}

void NegateNode::streamTo(SourceStream &s) const
{
    s << "- " << expr;
}

void BitwiseNotNode::streamTo(SourceStream &s) const
{
    s << '~' << expr;
}

void LogicalNotNode::streamTo(SourceStream &s) const
{
    s << '!' << expr;
}

void BinaryOperatorNode::streamTo(SourceStream &s) const
{
    const char *opStr;
    switch (oper) {
    case OpMult:
        opStr = " * ";
        break;
    case OpDiv:
        opStr = " / ";
        break;
    case OpMod:
        opStr = " % ";
        break;
    case OpPlus:
        opStr = " + ";
        break;
    case OpMinus:
        opStr = " - ";
        break;
    case OpLShift:
        opStr = " << ";
        break;
    case OpRShift:
        opStr = " >> ";
        break;
    case OpURShift:
        opStr = " >>> ";
        break;
    case OpLess:
        opStr = " < ";
        break;
    case OpGreaterEq:
        opStr = " >= ";
        break;
    case OpGreater:
        opStr = " > ";
        break;
    case OpLessEq:
        opStr = " <= ";
        break;
    case OpIn:
        opStr = " in ";
        break;
    case OpInstanceOf:
        opStr = " instanceof ";
        break;
    case OpEqEq:
        opStr = " == ";
        break;
    case OpNotEq:
        opStr = " != ";
        break;
    case OpStrEq:
        opStr = " === ";
        break;
    case OpStrNEq:
        opStr = " !== ";
        break;
    case OpBitAnd:
        opStr = " & ";
        break;
    case OpBitXOr:
        opStr = " ^ ";
        break;
    case OpBitOr:
        opStr = " | ";
        break;
    default:
        assert(!"Unhandled case in BinaryOperatorNode::streamTo()");
        opStr = " ??? ";
        break;
    }
    s.append(expr1, opStr, expr2);
}

void BinaryLogicalNode::streamTo(SourceStream &s) const
{
    s.append(expr1, (oper == OpAnd ? " && " : " || "), expr2);
}

void ConditionalNode::streamTo(SourceStream &s) const
{
    s << logical << " ? ";
    s.append(expr1, " : ", expr2);
}

static void streamAssignmentOperatorTo(SourceStream &s, Operator oper)
{
    const char *opStr;
    switch (oper) {
    case OpEqual:
        opStr = " = ";
        break;
    case OpMultEq:
        opStr = " *= ";
        break;
    case OpDivEq:
        opStr = " /= ";
        break;
    case OpPlusEq:
        opStr = " += ";
        break;
    case OpMinusEq:
        opStr = " -= ";
        break;
    case OpLShift:
        opStr = " <<= ";
        break;
    case OpRShift:
        opStr = " >>= ";
        break;
    case OpURShift:
        opStr = " >>>= ";
        break;
    case OpAndEq:
        opStr = " &= ";
        break;
    case OpXOrEq:
        opStr = " ^= ";
        break;
    case OpOrEq:
        opStr = " |= ";
        break;
    case OpModEq:
        opStr = " %= ";
        break;
    default:
        opStr = " ?= ";
    }
    s << opStr;
}

void AssignNode::streamTo(SourceStream &s) const
{
    s << m_loc;
    streamAssignmentOperatorTo(s, m_oper);
    s << m_right;
//  s.append(m_ident, opStr, m_right);
}

void CommaNode::streamTo(SourceStream &s) const
{
    s.append(expr1, ", ", expr2);
}

void AssignExprNode::streamTo(SourceStream &s) const
{
    s << " = " << expr;
}

void VarDeclNode::streamTo(SourceStream &s) const
{
    s << ident << init;
}

void VarDeclListNode::streamTo(SourceStream &s) const
{
    s << "var " << var;
    for (VarDeclListNode *n = next.get(); n; n = n->next.get()) {
        s << ", " << n->var;
    }
}

void VarStatementNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << next << ';';
}

void BlockNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << '{' << SourceStream::Indent
      << source << SourceStream::Unindent << SourceStream::Endl << '}';
}

void ProgramNode::streamTo(SourceStream &s) const
{
    // we don't want braces here, unlike in the above
    s << source << SourceStream::Endl;
}

void EmptyStatementNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << ';';
}

void ExprStatementNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << expr << ';';
}

void IfNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << "if (" << expr << ')' << SourceStream::Indent
      << statement1 << SourceStream::Unindent;
    if (statement2)
        s << SourceStream::Endl << "else" << SourceStream::Indent
          << statement2 << SourceStream::Unindent;
}

void DoWhileNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << "do " << SourceStream::Indent
      << statement << SourceStream::Unindent << SourceStream::Endl
      << "while (" << expr << ");";
}

void WhileNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << "while (" << expr << ')' << SourceStream::Indent
      << statement << SourceStream::Unindent;
}

void ForNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << "for ("
      << expr1
      << "; " << expr2
      << "; " << expr3
      << ')' << SourceStream::Indent << statement << SourceStream::Unindent;
}

void ForInNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << "for (";
    if (varDecl) {
        s << "var " << varDecl;
    } else {
        s << lexpr;
    }

    s << " in " << expr << ')' << SourceStream::Indent
      << statement << SourceStream::Unindent;
}

void ContinueNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << "continue";
    if (!ident.isNull()) {
        s << ' ' << ident;
    }
    s << ';';
}

void BreakNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << "break";
    if (!ident.isNull()) {
        s << ' ' << ident;
    }
    s << ';';
}

void ReturnNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << "return";
    if (value) {
        s << ' ' << value;
    }
    s << ';';
}

void WithNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << "with (" << expr << ") "
      << statement;
}

void CaseClauseNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl;
    if (expr) {
        s << "case " << expr;
    } else {
        s << "default";
    }
    s << ':' << SourceStream::Indent;
    if (source) {
        s << source;
    }
    s << SourceStream::Unindent;
}

void ClauseListNode::streamTo(SourceStream &s) const
{
    for (const ClauseListNode *n = this; n; n = n->getNext()) {
        s << n->getClause();
    }
}

void CaseBlockNode::streamTo(SourceStream &s) const
{
    for (const ClauseListNode *n = list1.get(); n; n = n->getNext()) {
        s << n->getClause();
    }
    if (def) {
        s << def;
    }
    for (const ClauseListNode *n = list2.get(); n; n = n->getNext()) {
        s << n->getClause();
    }
}

void SwitchNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << "switch (" << expr << ") {"
      << SourceStream::Indent << block << SourceStream::Unindent
      << SourceStream::Endl << '}';
}

void LabelNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << label << ':' << SourceStream::Indent
      << statement << SourceStream::Unindent;
}

void ThrowNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << "throw " << expr << ';';
}

void TryNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << "try " << tryBlock;
    if (catchBlock) {
        s << SourceStream::Endl << "catch (" << exceptionIdent << ')' << catchBlock;
    }
    if (finallyBlock) {
        s << SourceStream::Endl << "finally " << finallyBlock;
    }
}

void ParameterNode::streamTo(SourceStream &s) const
{
    s << id;
    for (ParameterNode *n = next.get(); n; n = n->next.get()) {
        s << ", " << n->id;
    }
}

void FuncDeclNode::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << "function " << ident << '(' << param << ')' << body;
}

void FuncExprNode::streamTo(SourceStream &s) const
{
    s << "function " << ident << '(' << param << ')' << body;
}

void SourceElementsNode::streamTo(SourceStream &s) const
{
    for (const SourceElementsNode *n = this; n; n = n->next.get()) {
        s << n->node;
    }
}

void PackageNameNode::streamTo(SourceStream &s) const
{
    if (names) {
        s << names << '.';
    }
    s << id;
}

void ImportStatement::streamTo(SourceStream &s) const
{
    s << SourceStream::Endl << "import ";
    if (!al.isEmpty()) {
        s << al << " = ";
    }
    s << name << (wld ? ".*;" : ";");
}

