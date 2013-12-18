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

#ifndef CODE_PRINTER_H
#define CODE_PRINTER_H

#include <iostream>
#include <vector>
#include <string>

using std::ostream;
using std::vector;
using std::string;

typedef std::vector<string> StringList;

// This class generates a declaration for an enum,
// and also declares & defines instrospection tables.
class Enum {
public:
    Enum(const string& name, const string& prefix, StringList values):
        name(name), prefix(prefix), values(values)
    {}

    void printDeclaration(ostream& cppStream);
    void printDefinition (ostream& hStream);
private:
    string name;
    string prefix;
    StringList values;
};

// A helper for bracing, commas, etc., of constant array tables
class Array {
public:
    Array(ostream& out, const std::string& type, const std::string &name);
    void item(const std::string& val, const std::string& pendingComment = "");
    void endArray();
    ~Array();
private:
    ostream&    out;
    std::string comment;
    bool ended, first;
};

enum CodeStream {
    OpH,
    OpCpp,
    MaCpp
};

class CodePrinter
{
public:
    CodePrinter(ostream* hStream, ostream* cppStream, ostream* mStream):
        hStream(hStream), cppStream(cppStream), mStream(mStream) {}

    void printCode(ostream& out, int baseIndent, const string& code, int baseLine);

    ostream& operator()(CodeStream stream) {
        switch (stream) {
            case OpH:
                return *hStream;
            case OpCpp:
                return *cppStream;
            default:
                return *mStream;
        }
    }

    static std::string stringFromInt(int val);

    // Indented print to machine.cpp.
    ostream& mInd(int ind);

    void issueError(const string& err);
    
private:
    ostream* hStream;
    ostream* cppStream;
    ostream* mStream;
};

#endif
// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
