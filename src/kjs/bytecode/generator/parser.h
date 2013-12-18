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

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

#include "types.h" // for various flags

#include <vector>

enum ParamFlags {
    Param_NoImm = 1,
    Param_NoReg = 2,
    Param_Exact = 4
};

enum OpFlags {
    Op_EndsBB = 1,
    Op_Hint   = 2
};

struct Parameter {
    string   name;
    string   typeName; // name of the type of the parameter, set by the parser
    Type     type;     // the actual type of the parameter, resolved by the TableBuilder.
    unsigned flags;

    Parameter(): flags(0) {}
};

class Parser
{
public:
    Parser(istream *stream);
    virtual ~Parser();

    void parse();
private:
    // Note: signatures here are just a list of strings;
    // the last one is the return type

    virtual void handleType(const string &type, const string &nativeName, unsigned flags) = 0;

    virtual void handleConversion(const string &runtimeRoutine, int codeLine,
                                  unsigned flags, const string &from, const string &to,
                                  int tileCost, int registerCost) = 0;

    virtual void handleOperation(const string &name, unsigned flags) = 0;
    virtual void handleImpl(const string &fnName, const string &code, int codeLine, int cost,
                            const string &retType, vector<Parameter> sig) = 0;
    virtual void handleTile(const string &fnName, StringList sig) = 0;

    struct Flag {
        const char *name;
        unsigned    value;
    };

    // Matches flags specified as a zero-terminated pair array above,
    // and returns their or. In syntax, they look like
    // [flag1, flag2, flag3] and are optional.
    unsigned matchFlags(const Flag *permittedFlags);

    // These unconditionally parse items of given type.
    string matchIdentifier();
    void   matchCode(std::string *stringOut, int *lineOut);
    int    matchNumber();
    void   match(Lexer::TokenType t);

    // These conditionally consume given token if it's there,
    // and return true if so.
    bool check(Lexer::TokenType t);

    void parseType();
    void parseConversion();
    void parseOperation();
    void parseImpl();
    void parseTile();

    bool tokenLoaded;
    Lexer::Token nextToken;

    void issueError(const string &msg);

    bool   hadError;

    Lexer *lexer;

    Lexer::Token peekNext();
    Lexer::Token getNext();
};

#endif

