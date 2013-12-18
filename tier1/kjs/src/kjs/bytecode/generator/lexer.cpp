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

#include "lexer.h"
#include <cctype>

#ifndef _WIN32
using std::isspace;
#endif

#include <wtf/ASCIICType.h>
using namespace WTF;

const char EndOfFileChar = 0;

Lexer::Lexer(istream* _stream): stream(_stream), charLoaded(false), lineNum(0)
{
    keywords["type"]        = Type;
    keywords["conversion" ] = Conversion;
    keywords["register"]    = Register;
    keywords["operation"]   = Operation;
    keywords["costs"]       = Costs;
    keywords["impl"]        = Impl;
    keywords["tile"]        = Tile;
    keywords["as"]          = As;
    keywords["runtime"]     = Runtime;
    lineNum = 1;
}

Lexer::Token Lexer::lexComment()
{
    char in = getNext();
    if (in == '/') {
        // Single-line comment -- read until the end of line (or file)
        do {
            in = getNext();
        } while (in != '\n' && in != EndOfFileChar);
    } else if (in == '*') {
        // Multi-line comment --- scan until  */
        do {
            in = getNext();
            if (in == EndOfFileChar)
                return Token(Error, "Unterminated multiline comment");
        }
        while (!(in == '*' && peekNext() == '/'));
        getNext(); // Eat the /
    } else {
        return Token(Error, string("/ can only start comments, but is followed by: ") + in);
    }

    // Wee. Worked fine. Recurse to get next stuff
    return nextToken();
}

Lexer::Token Lexer::nextToken()
{
    char begin;

    // Skip any whitespace characters..
    do {
        begin = getNext();
    }
    while (isspace(begin));

    if (begin == EndOfFileChar)
        return Token(EndOfFile);

    // Check for simple chars..
    if (begin == '{')
        return Token(LBrace);
    else if (begin == '}')
        return Token(RBrace);
    if (begin == '(')
        return Token(LParen);
    else if (begin == ')')
        return Token(RParen);
    else if (begin == ':') {
        if (peekNext() != ':') {
            return Token(Colon);
        } else {
            getNext();
            return Token(Scope);
        }
    } else if (begin == ';')
        return Token(SemiColon);
    else if (begin == '*')
        return Token(Star);
    else if (begin == ',')
        return Token(Comma);
    else if (begin == ']')
        return Token(RBracket);
    else if (begin == '[' && peekNext() != '[')
        return Token(LBracket);

    // =>
    if (begin == '=') {
        char c2 = getNext();
        if (c2 == '>')
            return Token(Arrow);
        else
            return Token(Error, "- not part of ->");
    }

    // Check for comments..
    if (begin == '/')
        return lexComment();

    // Numbers
    if (isASCIIDigit(begin)) {
        string text;
        text += begin;
        while (isASCIIDigit(peekNext()))
            text += getNext();
        return Token(Number, text);
    }

    // Code..
    if (begin == '[') {
        char next = getNext();
        if (next != '[')
            return Token(Error, string("[ continued with:") + next);
        int line = lineNumber();
        string text;
        while(true) {
            char letter = getNext();
            if (letter == EndOfFileChar)
                return Token(Error, "Unterminated code fragment");
            if (letter == ']' && peekNext() == ']') {
                getNext(); //Eat 2nd ']'
                return Token(Code, text, line);
            }
            text += letter;
        }
    }

    // Identifiers
    if (isASCIIAlpha(begin)) {
        string text;
        text = begin;
        while (isASCIIAlphanumeric(peekNext()) || peekNext() == '_')
            text += getNext();

        TokenType t = Ident;
        if (keywords.find(text) != keywords.end())
            t = keywords[text];
        return Token(t, text);
    } else {
        return Token(Error, string("Invalid start of token:") + begin);
    }
}

char Lexer::peekNext()
{
    if (charLoaded)
        return nextChar;

    nextChar = stream->get();

    if (stream->eof())
        return EndOfFileChar;

    if (nextChar == '\n')
        ++lineNum;
    charLoaded = true;
    return nextChar;
}

char Lexer::getNext()
{
    if (charLoaded) {
        charLoaded = false;
        return nextChar;
    }

    char in = stream->get();

    if (stream->eof()) {
        // Make sure to let peekNext know.
        nextChar   = EndOfFileChar;
        charLoaded = true;
        return EndOfFileChar;
    }

    if (in == '\n')
        ++lineNum;

    return in;
}

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
