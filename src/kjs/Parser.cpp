// -*- c-basic-offset: 4 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003, 2006, 2007 Apple Inc.
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

#include "Parser.h"

#include "lexer.h"
#include "nodes.h"
#include <wtf/HashSet.h>
#include <wtf/Vector.h>

#ifdef KJS_VERBOSE
#include <stdio.h>
#endif

extern int kjsyyparse();

namespace KJS {

Parser::Parser()
    : m_sourceId(0)
{
}

PassRefPtr<ProgramNode> Parser::parseProgram(const UString& sourceURL, int startingLineNumber,
    const UChar* code, unsigned length,
    int* sourceId, int* errLine, UString* errMsg)
{
    parse(sourceURL, startingLineNumber, code, length, sourceId, errLine, errMsg);
    return m_progNode.release();
}

static HashSet<Node*>* nodeCycles;


void Parser::noteNodeCycle(Node *node)
{
    if (!nodeCycles)
        nodeCycles = new HashSet<Node*>;
    nodeCycles->add(node);
}

void Parser::removeNodeCycle(Node *node)
{
    ASSERT(nodeCycles);
    nodeCycles->remove(node);
}

static void clearNewNodes()
{
    if (nodeCycles) {
        for (HashSet<Node*>::iterator it = nodeCycles->begin(); it != nodeCycles->end(); ++it)
            (*it)->breakCycle();
        delete nodeCycles;
        nodeCycles = 0;
    }
    Node::clearNewNodes();
}

PassRefPtr<FunctionBodyNode> Parser::parseFunctionBody(const UString& sourceURL, int startingLineNumber,
    const UChar* code, unsigned length,
    int* sourceId, int* errLine, UString* errMsg)
{
    parse(sourceURL, startingLineNumber, code, length, sourceId, errLine, errMsg);
    return m_progNode.release();
}

void Parser::parse(const UString& sourceURL, int startingLineNumber,
    const UChar* code, unsigned length,
    int* sourceId, int* errLine, UString* errMsg)
{
    pushFunctionContext(0);
    
    ASSERT(!m_progNode);

    if (errLine)
        *errLine = -1;
    if (errMsg)
        *errMsg = 0;

    Lexer& lexer = KJS::lexer();

    lexer.setCode(sourceURL, startingLineNumber, code, length);
    m_sourceId++;
    if (sourceId)
        *sourceId = m_sourceId;

    // Enable this and the #define YYDEBUG in grammar.y to debug a parse error
    //extern int kjsyydebug;
    //kjsyydebug=1;

    int parseError = kjsyyparse();

    bool lexError = lexer.sawError();
    lexer.clear();

    clearNewNodes();

    if (parseError || lexError) {
        if (errLine)
            *errLine = lexer.lineNo();
        if (errMsg)
            *errMsg = "Parse error";
        m_progNode = 0;
        return;
    }

#ifdef KJS_VERBOSE
  fprintf( stderr, "%s\n", m_progNode->toString().ascii() );
#endif
}

void Parser::didFinishParsing(PassRefPtr<ProgramNode> progNode)
{
    m_progNode = progNode;
}

Parser& parser()
{
    // ASSERT(JSLock::currentThreadIsHoldingLock());

    static Parser staticParser;
    return staticParser;
}

} // namespace KJS
