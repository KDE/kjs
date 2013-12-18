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

#include "types.h"

#include <stdlib.h>
#include <iostream>
#include "assert.h"
#include <cctype>
#include <cstdio>
#include <wtf/ASCIICType.h>

#include "tablebuilder.h"

using namespace std;

// # of bits store 'vals' values, e.g. 3 for 8, etc.
static unsigned neededBits(unsigned vals)
{
    unsigned bits = 1;
    while ((1U << bits) < vals) {
        ++bits;
    }
    return bits;
}

TypeTable::TypeTable(TableBuilder *instrBuilder, CodePrinter &out):
    instrBuilder(instrBuilder), out(out)
{
    // Builtin stuff...
    conversionNames.push_back("NoConversion");
    conversionNames.push_back("NoOp");

    // Special ones for checked; opcodes.cpp.in has code to generate these.
    conversionNames.push_back("I_R_Int32_Value");
    conversionNames.push_back("I_R_Number_Value");
}

void TypeTable::generateCode()
{
    // Types... First we just want to list them
    Enum typesEnum("OpType", "OpType_", typeNames);
    typesEnum.printDeclaration(out(OpH));
    typesEnum.printDefinition(out(OpCpp));

    // Also, print out the width array...
    Array widths(out(OpCpp), "const bool", "opTypeIsAlign8");
    for (unsigned t = 0; t < typeNames.size(); ++t) {
        const Type &type = types[typeNames[t]];
        widths.item(type.alignTo8() ? "true" : "false", type.name);
    }
    widths.endArray();

    // Conversion ops. Those go entirely in the .cpp
    Enum convOps("ConvOp", "Conv_", conversionNames);
    convOps.printDeclaration(out(OpH));
    convOps.printDefinition(out(OpCpp));

    out(OpCpp) << "struct ConvInfo {\n";
    out(OpCpp) << "    ConvOp routine;\n";
    out(OpCpp) << "    int    costCode;\n";
    out(OpCpp) << "};\n\n";

    // For conversion info, we use upper bit for register/immediate (immediate is set),
    // and then enough bits for the from/to types as the index.
    Array convArr(out(OpCpp), "ConvInfo", "conversions");
    printConversionInfo(convArr, rgConversions, true);
    printConversionInfo(convArr, imConversions, false);
    convArr.endArray();

    int numBits = neededBits(types.size());
    out(OpCpp) << "static inline const ConvInfo* getConversionInfo(bool immediate, OpType from, OpType to)\n{\n";
    out(OpCpp) << "    return &conversions[((int)immediate << " << (2 * numBits) << ")"
               << " | ((int)from << " << numBits << ") | (int)to];\n";
    out(OpCpp) << "}\n\n";

    // Emit inline conversion helpers based on the [[ code ]] specified
    // in codes.def
    for (unsigned c = 0; c < imConversionList.size(); ++c) {
        printConversionRoutine(imConversionList[c]);
    }

    // Now we generate a helper that invokes those.
    out(OpCpp) << "static bool emitImmediateConversion(ConvOp convType, OpValue* original, OpValue& out)\n{\n";
    out(OpCpp) << "    out.immediate = true;\n";
    out(OpCpp) << "    switch(convType) {\n";
    out(OpCpp) << "    case Conv_NoOp:\n";
    out(OpCpp) << "        out = *original;\n";
    out(OpCpp) << "        break;\n";
    for (unsigned c = 0; c < imConversionList.size(); ++c) {
        const ConversionInfo &inf = imConversionList[c];
        out(OpCpp) << "    case Conv_" << inf.name << ":\n";
        out(OpCpp) << "        out.type = OpType_" << inf.to.name << ";\n";
        out(OpCpp) << "        out.value." << inf.to.field() << " = "
                   << "convert" << inf.name << "(0, "
                   << "original->value." << inf.from.field() << ");\n";

        out(OpCpp) << "        break;\n";
    }

    out(OpCpp) << "    default:\n";
    out(OpCpp) << "        return false;\n";
    out(OpCpp) << "    }\n";
    out(OpCpp) << "    return true;\n";
    out(OpCpp) << "}\n\n";

    // Similar helper for simple register conversions, which actually emits ops
    out(OpCpp) << "static bool emitSimpleRegisterConversion(CompileState* comp, ConvOp convType, OpValue* original, OpValue& out)\n{\n";
    out(OpCpp) << "    switch(convType) {\n";
    out(OpCpp) << "    case Conv_NoOp:\n";
    out(OpCpp) << "        out = *original;\n";
    out(OpCpp) << "        break;\n";
    for (unsigned c = 0; c < rgConversionList.size(); ++c) {
        const ConversionInfo &inf = rgConversionList[c];
        out(OpCpp) << "    case Conv_" << inf.name << ":\n";
        out(OpCpp) << "        CodeGen::emitOp(comp, Op_" << inf.name << ", &out, original);\n";
        out(OpCpp) << "        break;\n";
    }
    out(OpCpp) << "    default:\n";
    out(OpCpp) << "        return false;\n";
    out(OpCpp) << "    }\n";
    out(OpCpp) << "    return true;\n";
    out(OpCpp) << "}\n\n";
}

void TypeTable::printConversionInfo(Array &outArr, map<string, map<string, ConversionInfo> > &table, bool reg)
{
    unsigned numBits = neededBits(types.size());
    unsigned fullRange = 1 << numBits;
    for (unsigned from = 0; from < fullRange; ++from) {
        for (unsigned to = 0; to < fullRange; ++to) {
            if (from < types.size() && to < types.size()) {
                string fromName = typeNames[from];
                string toName   = typeNames[to];

                string item;

                // For register conversion, we need it to be possible for source + dest to be in
                // registers. For immediate, we only require source, since dest will just go
                // into local value.
                bool representable;
                if (reg) {
                    representable = types[fromName].hasReg() && types[toName].hasReg();
                } else {
                    representable = types[fromName].hasImm();
                }

                if (from == to) {
                    item = "{Conv_NoOp, 0}";
                } else if (table[fromName].find(toName) != table[fromName].end() && representable) {
                    const ConversionInfo &inf = table[fromName][toName];

                    item = "{Conv_" + inf.name + ", ";
                    if (inf.flags & Conv_Checked) {
                        item += "Cost_Checked";
                    } else {
                        item += CodePrinter::stringFromInt(reg ? inf.cost : 0);
                    }
                    item += "}"; // krazy:exclude=doublequote_chars
                } else {
                    item = "{Conv_NoConversion, Cost_NoConversion}";
                }

                outArr.item(item, fromName + " => " + toName);
            } else {
                outArr.item("{Conv_NoConversion, Cost_NoConversion}");
            }
        } // for to..
    } // for from..
}

void TypeTable::printConversionRoutine(const ConversionInfo &conversion)
{
    out(OpH) << "ALWAYS_INLINE " << conversion.to.nativeName << " convert" << conversion.name
             << "(ExecState* exec, " << conversion.from.nativeName << " in)\n";
    out(OpH) << "{\n";
    out(OpH) << "    (void)exec;\n";
    out.printCode(out(OpH), 4, conversion.impl, conversion.codeLine);
    out(OpH) << "}\n\n";
}

void TypeTable::handleType(const string &type, const string &nativeName, unsigned flags)
{
    typeNames.push_back(type);
    Type t;
    t.name  = type;
    t.nativeName = nativeName;
    t.flags = flags;
    types[type] = t;
}

static string capitalized(const string &in)
{
    return WTF::toASCIIUpper(in[0]) + in.substr(1);
}

void TypeTable::handleConversion(const string &code, int codeLine,
                                 unsigned flags, const string &from, const string &to,
                                 int tileCost, int registerCost)
{
    // Compute the conversion names. The register one (if any) would also create an operation.
    string immName = "I" + capitalized(from) + "_" + capitalized(to); // krazy:exclude=doublequote_chars
    string regName = "R" + capitalized(from) + "_" + capitalized(to); // krazy:exclude=doublequote_chars

    // Register immediate conversion
    conversionNames.push_back(immName);
    ConversionInfo inf;
    inf.name    = immName;
    inf.cost    = tileCost;
    inf.flags   = flags;
    inf.impl     = code;
    inf.codeLine = codeLine;
    inf.from     = types[from];
    inf.to       = types[to];

    imConversions[from][to] = inf;
    imConversionList.push_back(inf);

    // ... and, if it exists, register one.
    if (flags & Conv_HaveReg) {
        conversionNames.push_back(regName);
        inf.name = regName;
        inf.cost = registerCost;
        inf.flags &= ~Conv_Checked; // 'checked' makes no sense here
        rgConversions[from][to] = inf;
        rgConversionList.push_back(inf);

        // We also generate the corresponding bytecode routine, using
        // the immediate conversion helper we'll emit in it.
        instrBuilder->handleOperation(regName, false);

        vector<Parameter> sig;
        Parameter param;
        param.name     = "in";
        param.typeName = from;
        param.flags    = Param_Exact;
        sig.push_back(param);

        string code = inf.to.nativeName + " out = convertI" + inf.name.substr(1) + "(exec, in);\n";
        code += "$$ = out;\n";
        instrBuilder->handleImpl("", code, codeLine, 0, to, sig);
    }
}

Type TypeTable::resolveType(const string &type)
{
    if (types.find(type) != types.end()) {
        return types[type];
    } else {
        out.issueError("Unknown type:" + type);
    }

    // return something in nonvoid function
    Type t;
    return t;
}

vector<Type> TypeTable::resolveSignature(const StringList &in)
{
    vector<Type> sig;
    for (unsigned c = 0; c < in.size(); ++c) {
        sig.push_back(resolveType(in[c]));
    }

    return sig;
}

ConversionInfo TypeTable::immConv(const Type &from, const Type &to)
{
    return imConversions[from.name][to.name];
}

