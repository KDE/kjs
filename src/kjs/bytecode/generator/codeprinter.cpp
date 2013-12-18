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
#include "codeprinter.h"
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include "assert.h"
#include <cctype>
#include <cstdio>
#include <wtf/ASCIICType.h>

using namespace std;

void Enum::printDeclaration(ostream& hStream)
{
    hStream << "enum " << name << " {\n";
    for (unsigned p = 0; p < values.size(); ++p) {
        hStream << "    " << prefix << values[p] << ",\n";
    }
    hStream << "    " << prefix << "NumValues\n";
    hStream << "};\n";
    hStream << "extern const char* const " << name << "Vals[];\n\n";
}

void Enum::printDefinition(ostream& cppStream)
{
    Array vals(cppStream, "const char* const", name + "Vals");
    for (unsigned p = 0; p < values.size(); ++p)
        vals.item("\"" + prefix + values[p] + "\"");
    vals.endArray();
}

Array::Array(ostream& out, const std::string& type, const std::string &name):
    out(out), ended(false), first(true)
{
    out << type << " " << name << "[] = {\n";
}

Array::~Array()
{
    assert(ended);
}

void Array::item(const std::string& val, const std::string& newComment)
{
    if (!first) {
        out << ", ";
        if (!comment.empty())
            out << "// " << comment;

        out << "\n";
    } else {
        first = false;
    }

    comment = newComment;

    out << "    " << val;
}

void Array::endArray()
{
    assert(!ended);
    ended = true;

    if (!comment.empty())
            out << "  // " << comment;
    out << "\n};\n\n";
}

void CodePrinter::issueError(const string& err)
{
    std::cerr << err << "\n";
    exit(-1);
}

std::string CodePrinter::stringFromInt(int val)
{
    std::stringstream out;
    out << val;
    return out.str();
}

ostream& CodePrinter::mInd(int ind)
{
    for (int i = 0; i < ind; ++i)
        *mStream << ' ';
    return *mStream;
}

static bool isWhitespaceString(const string& str)
{
    for (unsigned c = 0; c < str.length(); ++c) {
        if (!WTF::isASCIISpace(str[c]))
            return false;
    }

    return true;
}

static StringList splitLines(const string& in)
{
    StringList lines;
    string     curLine;
    for (unsigned c = 0; c < in.length(); ++c) {
        if (in[c] == '\n') {
            lines.push_back(curLine);
            curLine = "";
        } else {
            curLine += in[c];
        }
    }
    return lines;
}

void CodePrinter::printCode(ostream& out, int baseIndent, const string& code, int baseLine)
{
    StringList lines = splitLines(code);

    if (!lines.empty() && isWhitespaceString(lines.front())) {
        ++baseLine;
        lines.erase(lines.begin());
    }

    if (!lines.empty() && isWhitespaceString(lines.back()))
        lines.pop_back();

    out << "#line " << baseLine << " \"codes.def\"\n";

    // Compute "leading" whitespace, from codes.def indentation
    unsigned minWhiteSpace = 100000;
    for (unsigned c = 0; c < lines.size(); ++c) {
        const string& line = lines[c];
        if (isWhitespaceString(line))
            continue;

        unsigned ws = 0;
        while (ws < line.length() && WTF::isASCIISpace(line[ws]))
            ++ws;
        if (ws < minWhiteSpace)
            minWhiteSpace = ws;
    }

    // Print out w/it stripped, and replaced with the indent
    // specified by the generator
    for (unsigned c = 0; c < lines.size(); ++c) {
        const string& line = lines[c];
        if (line.length() < minWhiteSpace)
            out << "\n";
        else {
            for (int c = 0; c < baseIndent; ++c)
                out << ' ';
            out << line.substr(minWhiteSpace) << "\n";
        }
    }
}

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
