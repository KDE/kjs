/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2012 Bernd Buschinski (b.buschinski@googlemail.com)
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

#ifndef JSONLEXER_H
#define JSONLEXER_H

#include "ustring.h"


namespace KJS {

class ExecState;
class JSValue;
class JSObject;
class JSONParser;

namespace JSONParserState {
    enum ParserState {
        JSONValue = 1,
        JSONObject,
        JSONArray
    };

    enum TokenType {
        TokLBracket, // [
        TokRBracket, // ]
        TokLBrace,   // {
        TokRBrace,   // }
        TokString,
        TokIdentifier,
        TokNumber,
        TokColon,
        TokLParen,
        TokRParen,
        TokComma,
        TokTrue,
        TokFalse,
        TokNull,
        TokEnd,
        TokError };
};


class JSONLexer {
public:
    explicit JSONLexer(const UString& code);

    JSONParserState::TokenType next();
    JSONParserState::TokenType current();
    double currentNumber() const;
    UString currentString() const;

private:
    inline JSONParserState::TokenType lexString();
    inline JSONParserState::TokenType lexNumber();
    UChar parseEscapeChar(bool *error);

    UString m_code;
    int m_pos;

    //Token Data
    JSONParserState::TokenType m_type;
    UString m_stringToken;
    double m_numberToken;
};


class JSONParser {
public:
    explicit JSONParser(const UString& code);

    // Returns the root parsed JSValue*
    // or NULL on failure
    JSValue* tryParse(ExecState* exec);

private:
    JSValue* parse(ExecState* exec, JSONParserState::ParserState state = JSONParserState::JSONValue);
    inline bool nextParseIsEOF();

    JSONParserState::ParserState m_state;
    JSONLexer m_lexer;
};


} // namespace KJS

#endif // JSONLEXER_H
