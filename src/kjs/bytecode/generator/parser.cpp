/*
 *  A utilitity for building various tables and specializations for the
 *  KJS Frostbyte bytecode
 *
 *  Copyright (C) 2007, 2008 Maks Orlovich (maksim@kde.org)
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
#include "parser.h"

#include <cstdlib>
#include <wtf/ASCIICType.h>

/**
 This is a fairly straightforward affair. It's written in a
 recursive descent style, but the language is actually regular.

 The error recovert is even simpler: we just exit. That's it.
*/

Parser::Parser(istream *stream): tokenLoaded(false), hadError(false), lexer(new Lexer(stream))
{}

Parser::~Parser()
{
    delete lexer;
}

string Parser::matchIdentifier()
{
    Lexer::Token tok = getNext();
    if (tok.type == Lexer::Ident) {
        return tok.value;
    }
    issueError("Expected identifier, got:" + tok.toString(lexer));
    return "";
}

void Parser::matchCode(std::string *strOut, int *lineOut)
{
    Lexer::Token tok = getNext();
    if (tok.type == Lexer::Code) {
        *lineOut = tok.lineNum;
        *strOut  = tok.value;
        return;
    }
    issueError("Expected code, got:" + tok.toString(lexer));
}

int Parser::matchNumber()
{
    Lexer::Token tok = getNext();
    if (tok.type == Lexer::Number) {
        return std::atol(tok.value.c_str());
    }
    issueError("Expected number, got:" + tok.toString(lexer));
    return 0;
}

void Parser::match(Lexer::TokenType t)
{
    Lexer::Token tok = getNext();
    if (tok.type != t) {
        issueError("Expected " + Lexer::Token(t).toString(lexer) + " got:" + tok.toString(lexer));
    }
}

bool Parser::check(Lexer::TokenType t)
{
    if (peekNext().type == t) {
        getNext(); // tasty!
        return true;
    } else {
        return false;
    }
}

unsigned Parser::matchFlags(const Flag *permittedFlags)
{
    unsigned flagsVal = 0;
    if (check(Lexer::LBracket)) {
        while (true) {
            std::string flag;

            // We permit keywords to double as flags.
            if (peekNext().isKeyword()) {
                flag = getNext().toString(lexer);
            } else {
                flag = matchIdentifier();
            }

            // Lookup the name.
            bool found = false;
            for (int pos = 0; permittedFlags[pos].name; ++pos) {
                if (flag == std::string(permittedFlags[pos].name)) {
                    found = true;
                    flagsVal |= permittedFlags[pos].value;
                }
            }

            if (!found) {
                issueError("invalid flag:" + flag);
            }

            // Done or more?
            if (check(Lexer::RBracket)) {
                return flagsVal;
            } else {
                match(Lexer::Comma);
            }
        }
    }
    return 0;
}

void Parser::issueError(const string &msg)
{
    std::cerr << "Parse error:" << msg << " at about line:" << lexer->lineNumber() << "\n";
    std::exit(-1);
}

Lexer::Token Parser::peekNext()
{
    if (!tokenLoaded) {
        nextToken   = lexer->nextToken();
        tokenLoaded = true;
    }

    return nextToken;
}

Lexer::Token Parser::getNext()
{
    if (tokenLoaded) {
        tokenLoaded = false;
        return nextToken;
    }

    return lexer->nextToken();
}

void Parser::parse()
{
    Lexer::Token tok = peekNext();

    // The types are first..
    while (tok.type == Lexer::Type) {
        parseType();
        tok = peekNext();
    }

    // Now we may have conversions or operations
    while (tok.type == Lexer::Conversion || tok.type == Lexer::Operation) {
        if (tok.type == Lexer::Conversion) {
            parseConversion();
        } else {
            parseOperation();
        }
        tok = peekNext();
    }

    match(Lexer::EndOfFile);
}

void Parser::parseType()
{
    //type identifier:  nativeName *? [immediate?, register?, ?align8]?;
    match(Lexer::Type);

    string name = matchIdentifier();
    match(Lexer::Colon);
    string nativeName = matchIdentifier();

    if (nativeName == "const") {
        nativeName += " " + matchIdentifier();    // krazy:exclude=doublequote_chars
    }

    while (check(Lexer::Scope)) {
        nativeName += "::" + matchIdentifier();
    }

    if (check(Lexer::Star)) {
        nativeName += "*";    // krazy:exclude=doublequote_chars
    }

    const Flag typeFlags[] = {
        {"immediate", Type_HaveImm},
        {"register",  Type_HaveReg},
        {"align8",    Type_Align8},
        {nullptr, 0}
    };

    unsigned flags = matchFlags(typeFlags);
    match(Lexer::SemiColon);

    handleType(name, nativeName, flags);
}

void Parser::parseConversion()
{
    // conversion from =>  to { clauses .. }
    // clause := tile costs number; || impl [checked?, mayThrow?]? code; || register ident costs number;
    match(Lexer::Conversion);
    string from = matchIdentifier();
    match(Lexer::Arrow);
    string to = matchIdentifier();

    match(Lexer::LBrace);

    // impl clause info..
    const Flag conversionFlags[] = {
        {"checked",  Conv_Checked},
        {"mayThrow", Conv_MayThrow},
        {nullptr, 0}
    };

    unsigned flags = 0;
    string code;
    int    codeLine = 0;

    // tile clause info
    int tileCost = 0;

    // register clause info
    string registerIdent;
    int registerCost = 0;

    while (!check(Lexer::RBrace)) {
        if (check(Lexer::Impl)) {
            // impl [[code]]
            flags = matchFlags(conversionFlags);
            matchCode(&code, &codeLine);
        } else if (check(Lexer::Tile)) {
            // tile costs number;
            match(Lexer::Costs);
            tileCost = matchNumber();
            match(Lexer::SemiColon);
        } else if (check(Lexer::Register)) {
            //register costs number;
            flags |= Conv_HaveReg;
            match(Lexer::Costs);
            registerCost  = matchNumber();
            match(Lexer::SemiColon);
        } else {
            issueError("Invalid start of a clause within conversion block:" + peekNext().toString(lexer));
        }
    }

    handleConversion(code, codeLine, flags, from, to, tileCost, registerCost);
}

void Parser::parseOperation()
{
    // operation identifier { ... },  where ... is a list of impl or tile statements.
    match(Lexer::Operation);

    const Flag opFlags[] = {
        {"endsBB", Op_EndsBB},
        {"hint",   Op_Hint},
        {nullptr, 0}
    };

    std::string name = matchIdentifier();
    unsigned flags   = matchFlags(opFlags);

    handleOperation(name, flags);

    match(Lexer::LBrace);
    Lexer::Token tok = peekNext();
    while (tok.type == Lexer::Tile || tok.type == Lexer::Impl) {
        if (tok.type == Lexer::Tile) {
            parseTile();
        } else {
            parseImpl();
        }
        tok = peekNext();
    }
    match(Lexer::RBrace);
}

void Parser::parseImpl()
{
    match(Lexer::Impl);
    // impl identifier identifier? ( paramList? ) code
    // paramList := ident ident
    // paramList := ident ident , paramList

    string ret = matchIdentifier();

    string fn;
    if (peekNext().type == Lexer::Ident) {
        fn = matchIdentifier();
    }
    match(Lexer::LParen);

    const Flag paramFlags[] = {
        {"noimm", Param_NoImm},
        {"noreg", Param_NoReg},
        {"exact", Param_Exact},
        {nullptr, 0}
    };

    // Parse parameter types and names, if any..
    vector<Parameter> params;
    while (peekNext().type != Lexer::RParen) {
        Parameter param;
        param.typeName = matchIdentifier();
        param.flags    = matchFlags(paramFlags);
        param.name     = matchIdentifier();

        params.push_back(param);

        if (!check(Lexer::Comma)) {
            break;
        }

        // Make sure we have an ident next, and not an rparen..
        if (peekNext().type != Lexer::Ident) {
            issueError("Parameter signature in impl doesn't start with an identifier!");
        }
    }
    match(Lexer::RParen);

    int cost = 0;
    if (peekNext().type == Lexer::Costs) {
        getNext();
        cost = matchNumber();
    }

    int codeLine;
    string code;
    matchCode(&code, &codeLine);

    handleImpl(fn, code, codeLine, cost, ret, params);
}

void Parser::parseTile()
{
    // tile signature as identifier;
    match(Lexer::Tile);

    StringList paramSigs;
    match(Lexer::LParen);
    while (peekNext().type != Lexer::RParen) {
        paramSigs.push_back(matchIdentifier());
        if (peekNext().type != Lexer::Comma) {
            break;
        }
        getNext(); // Eat the comma..
        // Make sure we have an ident next, and not an rparen..
        if (peekNext().type != Lexer::Ident) {
            issueError("Parameter signature in tile doesn't start with an identifier!");
        }
    }

    match(Lexer::RParen);

    match(Lexer::As);
    string     fn = matchIdentifier();
    handleTile(fn, paramSigs);
    match(Lexer::SemiColon);
}

