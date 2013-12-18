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

#ifndef TYPES_H
#define TYPES_H

#include <iostream>
#include <vector>
#include <map>

#include "codeprinter.h"

using std::ostream;
using std::vector;
using std::map;

enum TypeFlags {
    Type_HaveImm = 1,
    Type_HaveReg = 2,
    Type_Align8  = 4
};

enum ConvFlags {
    Conv_NoFlags,
    Conv_HaveReg  = 2,
    Conv_Checked  = 4,
    Conv_MayThrow = 8
};

struct Type
{
    string name;
    string nativeName;

    unsigned flags;

    // may not be the same as Type_Align8 in the feature..
    bool alignTo8() const {
        return (flags & Type_Align8) == Type_Align8;
    }

    bool hasReg() const {
        return (flags & Type_HaveReg) == Type_HaveReg;
    }

    bool hasImm() const {
        return (flags & Type_HaveImm) == Type_HaveImm;
    }

    // field in store cells to access for this type
    string field() const {
        return ((flags & Type_Align8) ? "wide" : "narrow") +
                std::string(".") + name + "Val";
    }

    bool operator==(const Type& other) const {
        return name == other.name;
    }
};

struct ConversionInfo
{
    string name;
    string impl;
    int  cost;   // for w/in tile for immediate, for external for reg
    unsigned flags;
    Type from;
    Type to;
    int codeLine;

    ConversionInfo(): cost(0), flags(Conv_NoFlags)
    {} //Can be called for trivial conversion
};

class TableBuilder;

// This class is responsible for managing types & conversions, and generating
// conversion-selection routines. 
class TypeTable
{
public:
    TypeTable(TableBuilder* instrBuilder, CodePrinter& out);

    void generateCode();
    
    void handleType(const string& type, const string& nativeName, unsigned flags);
    void handleConversion(const string& runtimeRoutine, int codeLine,
                          unsigned flags, const string& from, const string& to,
                          int tileCost, int registerCost);

    // issues error if there is a problem..
    vector<Type> resolveSignature(const StringList& in);
    Type         resolveType(const string& type);

    ConversionInfo immConv(const Type& from, const Type& to);
private:
    TableBuilder* instrBuilder;
    CodePrinter&  out;
    void printConversionInfo(Array& array, map<string, map<string, ConversionInfo> >& table, bool reg);

    void printConversionRoutine(const ConversionInfo& conversion);

    map<string, Type> types;
    StringList        typeNames;

    StringList conversionNames;
    vector<ConversionInfo> imConversionList;
    vector<ConversionInfo> rgConversionList;

    map<string, map<string, ConversionInfo> > imConversions;
    map<string, map<string, ConversionInfo> > rgConversions;
};

#endif
// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
