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

#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <string>
#include <map>

using std::string;
using std::map;
using std::istream;

class Lexer
{
public:
    enum TokenType {
        Ident,
        Number,
        Code,
        LBrace, // {
        RBrace, // }
        LParen, // (
        RParen, // )
        LBracket, // [
        RBracket, // ]
        Colon,  // :
        Scope,  // ::
        SemiColon, // ;
        Star,      // *
        Arrow,     // =>
        Comma,     // ,
        Error,
        EndOfFile,
        // Keywords:
        Type,
        Conversion,
        Register,
        Operation,
        Costs,
        Impl,
        Tile,
        As,
        Runtime
    };

    struct Token {
        TokenType type;
        string    value;

        int lineNum; //only set for code tokens.

        Token() : type(Error), value("Uninitialized token") {}
        Token(TokenType t): type(t) {}
        Token(TokenType t, const string& v, int line = -1): type(t), value(v), lineNum(line) {}

        bool isKeyword() const {
            return type > EndOfFile;
        }

        string toString(Lexer* lex)
        {
            switch (type) {
            case LBrace:
                return "'{'";
            case RBrace:
                return "'}'";
            case LBracket:
                return "'['";
            case RBracket:
                return "']'";
            case LParen:
                return "'('";
            case RParen:
                return "')'";
            case Scope:
                return "'::'";
            case Colon:
                return "':'";
            case SemiColon:
                return "';'";
            case Star:
                return "'*'";
            case Arrow:
                return "'=>'";
            case Comma:
                return "','";
            case EndOfFile:
                return "<End of File>";
            case Error:
                return "<Lex Error:" + value + ">";
            case Code:
                return "[[" + value + "]]";
            case Ident:
            case Number:
                return value;
            default: {
                    // keywords
                    for (map<string, TokenType>::iterator it = lex->keywords.begin();
                         it != lex->keywords.end(); ++it) {

                         if (it->second == type)
                            return it->first;
                    }
                    return "???";
                } // default :
            } // switch(type)
        }
    };

    Lexer(istream* _stream);

    Token nextToken();

    int lineNumber() const { return lineNum; }
private:
    friend struct Token;
    Token lexComment();

    char peekNext();
    char getNext();

    istream* stream;

    bool  charLoaded;
    char  nextChar;
    int   lineNum;

    map<string, TokenType> keywords;
};

#endif
// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
