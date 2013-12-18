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

#include "jsonlexer.h"

#include <stack>

#include "lexer.h"
#include "object.h"

#include "wtf/Assertions.h"

// #define JSONLEXER_DEBUG_VERBOSE

namespace KJS {

using namespace JSONParserState;

static const unsigned short InvalidJSONUnicode = 0x001F;

static inline bool isDecimalDigit(const UChar &c)
{
    return (c.uc >= '0' && c.uc <= '9');
}

static inline bool isHexDigit(const UChar& c)
{
  return (isDecimalDigit(c) ||
          (c.uc >= 'a' && c.uc <= 'f') ||
          (c.uc >= 'A' && c.uc <= 'F'));
}

static inline bool isJSONWhiteSpace(const UChar& c)
{
    //ECMA Edition 5.1r6 - 15.12.1.1 - Syntax
    switch (c.uc) {
        case 0x0020: //SP
        case 0x0009: //TAB
        case 0x000A: //LF
        case 0x000D: //CR
            return true;
        default:
            return false;
    }
}

#ifdef JSONLEXER_DEBUG_VERBOSE
static inline UString tokenToString(TokenType type)
{
    switch (type) {
        case TokLBracket: return UString("TokLBracket");
        case TokRBracket: return UString("TokRBracket");
        case TokLBrace: return UString("TokLBrace");
        case TokRBrace: return UString("TokRBrace");
        case TokString: return UString("TokString");
        case TokIdentifier: return UString("TokIdentifier");
        case TokNumber: return UString("TokNumber");
        case TokColon: return UString("TokColon");
        case TokLParen: return UString("TokLParen");
        case TokRParen: return UString("TokRParen");
        case TokComma: return UString("TokComma");
        case TokTrue: return UString("TokTrue");
        case TokFalse: return UString("TokFalse");
        case TokNull: return UString("TokNull");
        case TokEnd: return UString("TokEnd");
        case TokError:  return UString("TokError");
    }
    ASSERT_NOT_REACHED();
    return UString("Default");
}

static inline UString parserStateToString(ParserState state)
{
    switch (state) {
        case JSONValue: return UString("JSONValue");
        case JSONObject: return UString("JSONObject");
        case JSONArray: return UString("JSONArray");
    }
    ASSERT_NOT_REACHED();
    return UString("Default");
}
#endif


// ------------------------------ JSONParser --------------------------------

JSONParser::JSONParser(const UString& code)
    : m_lexer(code)
{
#ifdef JSONLEXER_DEBUG_VERBOSE
        fprintf(stderr, "=============== new JSONParser ===============\n%s\n===============\n", code.ascii());
#endif
}

JSValue* JSONParser::tryParse(ExecState* exec)
{
    JSValue* ret = parse(exec);
    // If the syntax is correct, we never see the EOF, the last used token may be '}'.
    // But Syntax like "{} xyz" is also invalid, so we have to check if the next(last) token is EOF
    if (ret && nextParseIsEOF())
        return ret;
    return 0;
}

// helper function for adding a value to the object.
// the arrayStack saves all added values and gives the correct array position.
// This function will return false for NULL value or on exception.
static inline bool addArrayItem(ExecState* exec, std::stack<JSValue*>* arrayStack, JSValue* value, JSObject* object)
{
    if (exec->hadException())
        return false;

    if (!value)
        return false;

    arrayStack->push(value);
    object->put(exec, arrayStack->size()-1, value);
    return true;
}

JSValue* JSONParser::parse(ExecState* exec, ParserState state)
{
    if (exec->hadException())
        return 0;

    ParserState tState = state;
    TokenType type = m_lexer.next();

    JSObject* object = 0;
    std::stack<JSValue*> arrayObjectStack;
    UString propertyName;

    // For parsing the Object, did we found a propertyName?
    // NOTE: empty propertynames are allowed.
    bool havePropertyName = false;
    // For parsing the Object/Array, checks if we really added/found a propertyName
    // before we find the comma ','
    bool propAdded = false;
    // For parsing the Array, remember if last found token is Comma
    bool lastFoundIsTokComma = false;

    while (type != TokEnd && type != TokError) {
#ifdef JSONLEXER_DEBUG_VERBOSE
        fprintf(stderr, "TokenType: %s \t State: %s\n", tokenToString(type).ascii(), parserStateToString(tState).ascii());
#endif

        switch (tState) {
            case JSONValue:
                switch (type) {
                    case TokLBracket:
                        object = static_cast<JSObject *>(exec->lexicalInterpreter()->builtinArray()->construct(exec, List::empty()));
                        tState = JSONArray;
                        break;
                    case TokLBrace:
                        object = static_cast<JSObject *>(exec->lexicalInterpreter()->builtinObject()->construct(exec, List::empty()));
                        tState = JSONObject;
                        break;
                    case TokString:
                        return jsString(m_lexer.currentString());
                    case TokNull:
                        return jsNull();
                    case TokTrue:
                        return jsBoolean(true);
                    case TokFalse:
                        return jsBoolean(false);
                    case TokNumber:
                        return jsNumber(m_lexer.currentNumber());
                    default:
                        // This can only happen on invalid syntax and with 0 return
                        // we tell the caller we got a syntax error.

                        // ASSERT_NOT_REACHED();
                        return 0;
                }
                break;
            case JSONObject: {
                // if we got called from JSONArray-TokLBrace we did not create an object.

                // In more detail for the following JSON String "[{}]"
                // If we are in parse with type=JSONArray and state=TokLBrace,
                // means we just found the "{" in the Array, and call parse(exec, JSONObject),
                // now in this call type=JSONObject, state=TokRBrace ("}") and, our new, object=0 (!)
                // We will finish the object and return it, but as object is null, we return 0.
                // which would be wrong, as empty objects are allowed.
                // In this case we just report invalid data.
                // But for JSON String like "[{"a":1}]", we end up using object(0)->putDirect
                // and crash.

                // In short, remove this line and we will crash.
                object = object ? object : static_cast<JSObject *>(exec->lexicalInterpreter()->builtinObject()->construct(exec, List::empty()));
                switch (type) {
                    case TokString: // PropertyName
                        if (havePropertyName)
                            return 0;
                        propertyName = m_lexer.currentString();
                        havePropertyName = true;
                        break;
                    case TokColon: {
                        if (!havePropertyName)
                            return 0;
                        JSValue* val = parse(exec, JSONValue);
                        if (!val)
                            return 0;

                        // use putDirect to by-pass __proto__
                        object->putDirect(Identifier(propertyName), val);
                        propertyName = "";
                        havePropertyName = false;
                        propAdded = true;
                        break;
                    }
                    case TokRBrace: //Finish Object
                        if (havePropertyName)
                            return 0;
                        return object;
                    case TokComma: // Next Property
                        if (!propAdded)
                            return 0;
                        propAdded = false;
                        break;
                    default:
                        // This can only happen on invalid syntax and with 0 return
                        // we tell the caller we got a syntax error.

                        // ASSERT_NOT_REACHED();
                        return 0;
                }
                break;
            }
            case JSONArray: {
                // if we got called from JSONArray-TokLBracket we did not create an object
                object = object ? object : static_cast<JSObject *>(exec->lexicalInterpreter()->builtinArray()->construct(exec, List::empty()));

                // Check for invalid Array syntax, like ["1" "2"]
                switch (type) {
                    case TokNumber:
                    case TokString:
                    case TokNull:
                    case TokTrue:
                    case TokFalse:
                    case TokLBrace:
                    case TokLBracket:
                        if (propAdded)
                            return 0;
                        propAdded = true;
                        lastFoundIsTokComma = false;
                        break;
                    default:
                        break;
                }

                switch (type) {
                    case TokRBracket: // Finish array
                        // Check for invalid syntax like "[1,]"
                        if (lastFoundIsTokComma)
                            return 0;
                        return object;
                    case TokNumber:
                        if (!addArrayItem(exec, &arrayObjectStack, jsNumber(m_lexer.currentNumber()), object))
                            return 0;
                        break;
                    case TokString:
                        if (!addArrayItem(exec, &arrayObjectStack, jsString(m_lexer.currentString()), object))
                            return 0;
                        break;
                    case TokNull:
                        if (!addArrayItem(exec, &arrayObjectStack, jsNull(), object))
                            return 0;
                        break;
                    case TokTrue:
                        if (!addArrayItem(exec, &arrayObjectStack, jsBoolean(true), object))
                            return 0;
                        break;
                    case TokFalse:
                        if (!addArrayItem(exec, &arrayObjectStack, jsBoolean(false), object))
                            return 0;
                        break;
                    case TokLBrace:
                        if (!addArrayItem(exec, &arrayObjectStack, parse(exec, JSONObject), object))
                            return 0;
                        break;
                    case TokLBracket:
                        if (!addArrayItem(exec, &arrayObjectStack, parse(exec, JSONArray), object))
                            return 0;
                        break;
                    case TokComma: // Skip Comma and parse next Array Element
                        // if we found a comma without a leading property, this is invalid syntax
                        if (!propAdded)
                            return 0;
                        propAdded = false;
                        lastFoundIsTokComma = true;
                        break;
                    default:
                        // This can only happen on invalid syntax and with 0 return
                        // we tell the caller we got a syntax error.

                        // ASSERT_NOT_REACHED();
                        return 0;
                }
                break;
            }
            default:
                ASSERT_NOT_REACHED();
                return 0;
        }
        type = m_lexer.next();
    }

    if (type == TokError) {
#ifdef JSONLEXER_DEBUG_VERBOSE
        fprintf(stderr, "WARNING: JSONParse ending with error!\n");
#endif
        return 0;
    }
    if (type == TokEnd) {
#ifdef JSONLEXER_DEBUG_VERBOSE
        fprintf(stderr, "WARNING: JSONParse ending with unexpected END!\n");
#endif
        return 0;
    }
    ASSERT_NOT_REACHED();
    return 0;
}


// ------------------------------ JSONLexer --------------------------------

JSONLexer::JSONLexer(const UString& code)
    : m_code(code),
      m_pos(0)
{
}

TokenType JSONLexer::current()
{
    return m_type;
}

double JSONLexer::currentNumber() const
{
    ASSERT(m_type == TokNumber);
    return m_numberToken;
}

UString JSONLexer::currentString() const
{
    ASSERT(m_type == TokString);
    return m_stringToken;
}

TokenType JSONLexer::lexString()
{
    UString string;
    const int codeSize = m_code.size();
    //skip first detected '"'
    ++m_pos;

    if (m_pos >= codeSize) {
        m_type = TokError;
        return m_type;
    }

    //while not at the end of the string '"'
    while (!(m_code[m_pos] == '"')) {
        UChar cur = m_code[m_pos];
        if (cur == UChar('\\')) {
            ++m_pos;
            bool error = false;
            string.append(parseEscapeChar(&error));
            if (error) {
                m_type = TokError;
                return m_type;
            }
        } else {
            if (cur.uc <= InvalidJSONUnicode) {
                m_type = TokError;
                return m_type;
            }
            string.append(cur);
            ++m_pos;
        }

        if (m_pos >= codeSize) {
            m_type = TokError;
            return m_type;
        }
    }

    m_type = TokString;
    m_stringToken = string;
    ++m_pos;
#ifdef JSONLEXER_DEBUG_VERBOSE
    fprintf(stderr, "JSONLexer::lexString: Pos:%d stringlength:%d string:%s\n", m_pos, string.size(), string.ascii());
#endif
    return m_type;
}

TokenType JSONLexer::lexNumber()
{
    const int start = m_pos;
    const int codeSize = m_code.size();

    // -?(0 | [1-9][0-9]*) ('.' [0-9]+)? ([eE][+-]? [0-9]+)?

    // -?
    if (m_pos < codeSize && m_code[m_pos] == '-')
        ++m_pos;

    // (0 | [1-9][0-9]*)
    if (m_pos < codeSize && m_code[m_pos] == '0') {
        ++m_pos;
    } else if (m_pos < codeSize) {
        while (m_pos < codeSize && isDecimalDigit(m_code[m_pos])) {
            ++m_pos;
        }
    } else {
        m_type = TokError;
        return m_type;
    }

    // ('.' [0-9]+)?
    if (m_pos < codeSize && m_code[m_pos] == '.') {
        ++m_pos;
        // [0-9]+
        if (m_pos >= codeSize || !isDecimalDigit(m_code[m_pos])) {
            m_type = TokError;
            return m_type;
        }
        ++m_pos;

        while (m_pos < codeSize && isDecimalDigit(m_code[m_pos]))
            ++m_pos;
    }

    //  ([eE][+-]? [0-9]+)?
    if (m_pos < codeSize && (m_code[m_pos] == 'e' || m_code[m_pos] == 'E')) { // [eE]
        ++m_pos;

        // [-+]?
        if (m_pos < codeSize && (m_code[m_pos] == '-' || m_code[m_pos] == '+'))
            ++m_pos;

        // [0-9]+
        if (m_pos >= codeSize || !isDecimalDigit(m_code[m_pos])) {
            m_type = TokError;
            return m_type;
        }

        ++m_pos;
        while (m_pos < codeSize && isDecimalDigit(m_code[m_pos]))
            ++m_pos;
    }

    m_numberToken = m_code.substr(start, m_pos-start).toDouble(false, false);
    m_type = TokNumber;
#ifdef JSONLEXER_DEBUG_VERBOSE
    fprintf(stderr, "Number: %f\n", m_numberToken);
#endif
    return m_type;
}

UChar JSONLexer::parseEscapeChar(bool* error)
{
    UChar cur = m_code[m_pos];
    switch (cur.uc) {
        case '"':
        case '\\':
        case '/':
            ++m_pos;
            return cur;
        case 'b':
            ++m_pos;
            return UChar('\b');
        case 'f':
            ++m_pos;
            return UChar('\f');
        case 'n':
            ++m_pos;
            return UChar('\n');
        case 'r':
            ++m_pos;
            return UChar('\r');
        case 't':
            ++m_pos;
            return UChar('\t');
        case 'u':
        {
            if ((m_code.size() - (m_pos+1)) < 4) {
                *error = true;
                return UChar(' ');
            }
            if (!isHexDigit(m_code[m_pos+1]) || !isHexDigit(m_code[m_pos+2]) ||
                !isHexDigit(m_code[m_pos+3]) || !isHexDigit(m_code[m_pos+4])) {
                *error = true;
                return UChar(' ');
            }

            UChar next = Lexer::convertUnicode(m_code[m_pos+1].uc, m_code[m_pos+2].uc, m_code[m_pos+3].uc, m_code[m_pos+4].uc);
            if (next.uc <= InvalidJSONUnicode)
            {
                *error = true;
                return UChar(' ');
            }

            *error = false;
            m_pos += 5;
            return next;
        }
        default:
            *error = true;
            return UChar(' ');
    }
}

//helper function, checks if "word" is in the "code" at "pos".
static inline bool isStringSequence(int pos, const UString& code, const UString& word)
{
    const int wordSize = word.size();
    if (pos + wordSize > code.size())
        return false;

    //Skip first, we already checked it
    for (int i = 1; i < wordSize; ++i) {
        if (code[pos+i].uc != word[i].uc)
            return false;
    }
    return true;
}

TokenType JSONLexer::next()
{
    while(true) {
        if (m_pos >= m_code.size()) {
            m_type = TokEnd;
            return m_type;
        }

        if (!isJSONWhiteSpace(m_code[m_pos])) {
            break;
        }
        ++m_pos;
    }

    m_type = TokError;

#ifdef JSONLEXER_DEBUG_VERBOSE
    fprintf(stderr, "JSONLexer::next current: %c \t\t pos: %d/%d\n", char(m_code[m_pos].uc), m_pos, m_code.size());
#endif

    switch (m_code[m_pos].uc) {
        case '[':
            m_type = TokLBracket;
            ++m_pos;
            return m_type;
        case ']':
            m_type = TokRBracket;
            ++m_pos;
            return m_type;
        case '(':
            m_type = TokLParen;
            ++m_pos;
            return m_type;
        case ')':
            m_type = TokRParen;
            ++m_pos;
            return m_type;
        case '{':
            m_type = TokLBrace;
            ++m_pos;
            return m_type;
        case '}':
            m_type = TokRBrace;
            ++m_pos;
            return m_type;
        case ',':
            m_type = TokComma;
            ++m_pos;
            return m_type;
        case ':':
            m_type = TokColon;
            ++m_pos;
            return m_type;
        case '"':
            return lexString();

        case 't':
            if (isStringSequence(m_pos, m_code, "true")) {
                m_type = TokTrue;
                m_pos += 4;
                return m_type;
            }
            break;
        case 'f':
            if (isStringSequence(m_pos, m_code, "false")) {
                m_type = TokFalse;
                m_pos += 5;
                return m_type;
            }
            break;
        case 'n':
            if (isStringSequence(m_pos, m_code, "null")) {
                m_type = TokNull;
                m_pos += 4;
                return m_type;
            }
            break;
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return lexNumber();
    }
    return m_type;
}

bool JSONParser::nextParseIsEOF()
{
    return m_lexer.next() == TokEnd;
}


} // namespace KJS
