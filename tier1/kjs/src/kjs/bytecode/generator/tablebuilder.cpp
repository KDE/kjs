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
#include "tablebuilder.h"
#include <stdlib.h>
#include <iostream>
#include "assert.h"
#include <cctype>
#include <cstdio>
#include <wtf/ASCIICType.h>

using namespace std;

static string strReplace(string where, string from, string to) {
    string res = where;
    size_t pos;
    while ((pos = res.find(from)) != string::npos) {
        res = res.substr(0, pos) + to + res.substr(pos + from.length());
    }
    return res;
}

TableBuilder::TableBuilder(istream* inStream, ostream* hStream,
                           ostream* cppStream, FileTemplate* fTemplate, ostream* mStream):
    Parser(inStream), out(hStream, cppStream, mStream), types(this, out), fTemplate(fTemplate)
{}

void TableBuilder::generateCode()
{
    parse();

    types.generateCode();

    // Operations
    Enum opNamesEnum("OpName", "Op_", operationNames);
    opNamesEnum.printDeclaration(out(OpH));
    opNamesEnum.printDefinition (out(OpCpp));

    // Enumerate all the variants..
    for (unsigned c = 0; c < operations.size(); ++c) {
        vector<bool> parIm;
        expandOperationVariants(operations[c], parIm);
    }

    // Return types for each..
    out(OpCpp) << "static const OpType opRetTypes[] = {\n";
    for (unsigned c = 0; c < operationNames.size(); ++c) {
        out(OpCpp) << "     OpType_" << operationRetTypes[operationNames[c]];
        if (c  != operationNames.size() - 1)
            out(OpCpp) << ",";
        out(OpCpp) << " //" << operationNames[c] << "\n";
    }
    out(OpCpp) << "};\n\n";

    // Now we have all our bytecode names... Whee.
    Enum opByteCodesEnum("OpByteCode", "OpByteCode_", variantNames);
    opByteCodesEnum.printDeclaration(out(OpH));
    opByteCodesEnum.printDefinition (out(OpCpp));

    // We can now emit the actual tables...

    // ... first descriptors for each bytecode op..
    out(OpCpp) << "const Op opsForOpCodes[] = {\n";
    for (unsigned c = 0; c < variants.size(); ++c) {
        const OperationVariant& variant = variants[c];
        if (variant.needsPadVariant)
            dumpOpStructForVariant(variant, true, variant.needsPadVariant, true);
        dumpOpStructForVariant(variant, false, variant.needsPadVariant, c != variants.size() - 1);
    }
    out(OpCpp) << "};\n\n";

    // then variant tables for each main op..
    for (unsigned c = 0; c < operationNames.size(); ++c) {
        const string& opName = operationNames[c];
        out(OpCpp) << "static const Op* const op" << opName << "Variants[] = {";
        StringList variants = variantNamesForOp[opName];
        for (unsigned v = 0; v < variants.size(); ++v) {
            out(OpCpp) << "&opsForOpCodes[OpByteCode_" << variants[v] << "], ";
        }
        out(OpCpp) << "0};\n";
    }
    out(OpCpp) << "\n";

    out(OpCpp) << "const Op* const* const opSpecializations[] = {\n";
    for (unsigned o = 0; o < operationNames.size(); ++o) {
        out(OpCpp) << "    op" << operationNames[o] << "Variants";
        if (o != (operationNames.size() - 1))
            out(OpCpp) << ",";
        out(OpCpp) << "\n";
    }
    out(OpCpp) << "};\n\n";

    // Jump table, if needed
    Array jumps(out(MaCpp), "static void*", "kjsVMOpHandlers");
    for (unsigned c = 0; c < variants.size(); ++c) {
        const OperationVariant& var = variants[c];
        if (var.needsPadVariant)
            jumps.item("__extension__ &&l" + var.sig + "_Pad");

        jumps.item("__extension__ &&l" + var.sig);
    }

    jumps.endArray();

    fTemplate->handleUntilGenerate();


    // Now, generate the VM loop.
    mInd(8) << "OpByteCode op = *reinterpret_cast<const OpByteCode*>(pc);\n";

    mInd(0) << "#ifdef USE_LABEL_VALS\n";
    mInd(8) << "goto *kjsVMOpHandlers[op];\n";
    mInd(8) << "{\n";
    mInd(0) << "#else\n";
    mInd(8) << "switch (op) {\n";
    mInd(0) << "#endif\n";
    for (unsigned c = 0; c < variants.size(); ++c) {
        const OperationVariant& var = variants[c];
        if (var.needsPadVariant) {
            mInd(12) << "handler(" + var.sig + "_Pad):\n";
            mInd(16) << "pc += 4;\n";
            mInd(16) << "// Fallthrough\n";
        }

        mInd(12) << "handler(" + var.sig + "): {\n";
        generateVariantImpl(var);
        mInd(12) << "}\n";
        mInd(12) << "continue;\n\n";
    }

    mInd(8) << "}\n\n";
}

void TableBuilder::handleType(const string& type, const string& nativeName, unsigned flags)
{
    types.handleType(type, nativeName, flags);
}

void TableBuilder::handleConversion(const string& code, int codeLine,
                                    unsigned flags, const string& from, const string& to,
                                    int tileCost, int registerCost)
{
    types.handleConversion(code, codeLine, flags, from, to, tileCost, registerCost);
}

void TableBuilder::handleOperation(const string& name, unsigned flags)
{
    operationNames.push_back(name);
    operationFlags = flags;
}

void TableBuilder::handleImpl(const string& fnName, const string& code, int codeLine, int cost,
                              const string& retType, vector<Parameter> sig)
{
    // If the return type isn't 'void', we prepend a destination register as a parameter in the encoding.
    // emitOp will convert things as needed
    vector<Parameter> extSig;
    if (retType != "void") {
        Parameter ret;
        ret.typeName = "reg";
        ret.name     = "fbDestReg";
        ret.flags    = 0;
        extSig.push_back(ret);
    }

    for (unsigned c = 0; c < sig.size(); ++c)
        extSig.push_back(sig[c]);

    // Now go through and resolve each type. These are also
    // the types of the params of this base op.
    Operation op;
    for (unsigned c = 0; c < extSig.size(); ++c) {
        extSig[c].type = types.resolveType(extSig[c].typeName);
        op.opParamTypes.push_back(extSig[c].type);
    }

    op.name           = operationNames.back();
    op.retType        = retType;
    operationRetTypes[op.name] = retType;
    op.isTile         = false;
    op.implementAs    = code;
    op.implParams     = extSig;
    op.codeLine       = codeLine;
    op.cost           = cost;
    op.flags          = operationFlags;
    operations.push_back(op);
    if (!fnName.empty())
        implementations[fnName] = op;
}

void TableBuilder::handleTile(const string& fnName, StringList sig)
{
    if (implementations.find(fnName) == implementations.end())
        out.issueError("Unknown implementation name " + fnName + " in a tile definition");
    const Operation& impl = implementations[fnName];

    // Add in a return reg if need be
    StringList extSig;
    if (impl.retType != "void")
        extSig.push_back("reg");

    for (unsigned c = 0; c < sig.size(); ++c)
        extSig.push_back(sig[c]);



    // Most of the stuff is the same as in the base impl,
    // but we have a different external signature, and are a tile.
    Operation op = impl;
    op.isTile       = true;
    op.opParamTypes = types.resolveSignature(extSig);

    // Now also include the cost of inline conversions.
    for (unsigned p = 0; p < op.opParamTypes.size(); ++p)
        op.cost += types.immConv(op.opParamTypes[p], op.implParams[p].type).cost;

    operations.push_back(op);
}

void TableBuilder::expandOperationVariants(const Operation& op, vector<bool>& paramIsIm)
{
    unsigned numParams = op.opParamTypes.size();
    if (paramIsIm.size() < numParams) {
        int paramPos = paramIsIm.size();
        bool hasIm  = (op.opParamTypes[paramPos].flags & Type_HaveImm) == Type_HaveImm;
        bool hasReg = (op.opParamTypes[paramPos].flags & Type_HaveReg) == Type_HaveReg;

        bool genIm  = hasIm;
        bool genReg = hasReg;

        // Don't generate non-register variants for tiles when possible.
        if (op.isTile && hasReg)
            genIm = false;

        // There may be hints saying not to generate some version
        if (op.implParams[paramPos].flags & Param_NoImm)
            genIm = false;

        if (op.implParams[paramPos].flags & Param_NoReg)
            genReg = false;

        if (genIm) {
            paramIsIm.push_back(true);
            expandOperationVariants(op, paramIsIm);
            paramIsIm.pop_back();
        }

        if (genReg) {
            paramIsIm.push_back(false);
            expandOperationVariants(op, paramIsIm);
            paramIsIm.pop_back();
        }
        return;
    }

    // Have a full variant... Build a signature.

    string sig = op.name;
    for (unsigned p = 0; p < numParams; ++p) {
        sig += "_"; // krazy:exclude=doublequote_chars
        sig += paramIsIm[p] ? "I" : "R";
        sig += op.opParamTypes[p].name;
    }

    // We may need padding if we have an immediate align8 param..
    bool needsPad = false;
    for (unsigned c = 0; c < numParams; ++c)
        needsPad |= (paramIsIm[c] & op.opParamTypes[c].alignTo8());

    OperationVariant var;
    var.sig = sig;
    var.op  = op;
    var.paramIsIm = paramIsIm;
    var.needsPadVariant = needsPad;

    // Build offset table, giving param positions..
    while (var.paramOffsets.size() < numParams) // no setSize in QList..
        var.paramOffsets.push_back(0);

    int pos = 4;
    // pad8/align ones go first.
    for (unsigned c = 0; c < numParams; ++c) {
        if (paramIsIm[c] & op.opParamTypes[c].alignTo8()) {
            var.paramOffsets[c] = pos;
            pos += 8;
        }
    }

    // Then the rest..
    for (unsigned c = 0; c < numParams; ++c) {
        if (!paramIsIm[c] || !op.opParamTypes[c].alignTo8()) {
            var.paramOffsets[c] = pos;
            pos += 4;
        }
    }
    var.size = pos;

    variants.push_back(var);
    if (needsPad) { // we put the pad before, due to the fallthrough idiom..
        string pSig = sig + "_Pad";
        variantNames.push_back(pSig);
        variantNamesForOp[op.name].push_back(pSig);
    }
    variantNames.push_back(sig);
    variantNamesForOp[op.name].push_back(sig);
}

void TableBuilder::dumpOpStructForVariant(const OperationVariant& variant, bool doPad,
                                          bool hasPadVariant, bool needsComma)
{
    out(OpCpp) << "    {";
    out(OpCpp) << "Op_" << variant.op.name << ", ";     // baseInstr..
    out(OpCpp) << "OpByteCode_" << (doPad ? variant.sig + "_Pad" : variant.sig) << ", "; // byteCode op
    out(OpCpp) << variant.op.cost << ", "; // uhm, cost. doh.
    int numParams = variant.op.opParamTypes.size();
    out(OpCpp) << numParams << ", "; // # of params

    // Param types.
    out(OpCpp) << "{";
    for (int p = 0; p < numParams; ++p) {
        out(OpCpp) << "OpType_" << variant.op.opParamTypes[p].name;
        if (p != numParams - 1)
            out(OpCpp) << ", ";
    }
    out(OpCpp) << "}, ";

    // Immediate flag..
    out(OpCpp) << "{";
    for (int p = 0; p < numParams; ++p) {
        out(OpCpp) << (variant.paramIsIm[p] ? "true" : "false");
        if (p != numParams - 1)
            out(OpCpp) << ", ";
    }
    out(OpCpp) << "}, ";

    // Exact params flag.
    out(OpCpp) << "{";
    for (int p = 0; p < numParams; ++p) {
        out(OpCpp) << ((variant.op.implParams[p].flags & Param_Exact) ? "true" : "false");
        if (p != numParams - 1)
            out(OpCpp) << ", ";
    }
    out(OpCpp) << "}, ";

    // Return type.
    out(OpCpp) << "OpType_" << variant.op.retType << ", ";

    int adjust = doPad ? 4 : 0; // padded version has 4 extra bytes,
                                // between the opcode and the first arg.
    // Size..
    out(OpCpp) << (variant.size + adjust) << ", ";

    // Offset table..
    out(OpCpp) << "{";
    for (int p = 0; p < numParams; ++p) {
        out(OpCpp) << (variant.paramOffsets[p] + adjust);
        if (p != numParams - 1)
            out(OpCpp) << ", ";
    }

    out(OpCpp) << "}, ";

    // Whether this is a padded version..
    out(OpCpp) << (doPad ? "true" : "false") << ", ";

    // And whether a padded version exists.
    out(OpCpp) << (hasPadVariant ? "true" : "false") << ", ";

    // Whether this ends a basic block.
    out(OpCpp) << (variant.op.flags & Op_EndsBB ? "true" : "false");

    if (needsComma)
        out(OpCpp) << "},\n";
    else
        out(OpCpp) << "}\n";
}

void TableBuilder::generateVariantImpl(const OperationVariant& variant)
{
    mInd(16) << "pc += " << variant.size << ";\n";
    mInd(16) << "const unsigned char* localPC = pc;\n";
    int numParams = variant.paramIsIm.size();
    for (int p = 0; p < numParams; ++p) {
        const Type& type  = variant.op.opParamTypes[p];
        bool        inReg = !variant.paramIsIm[p];
        int negPos = variant.paramOffsets[p] - variant.size;

        bool wideArg = !inReg && type.alignTo8();

        char negPosStr[64];
        std::sprintf(negPosStr, "%d", negPos);

        string accessString = "reinterpret_cast<const ";
        accessString += wideArg ? "WideArg" : "NarrowArg";
        accessString += "*>(localPC ";
        accessString += negPosStr;
        accessString += ")->";
        accessString += inReg ? string("regVal") : type.name + "Val";
        if (inReg) { // Need to indirect... The register value is actually an offset, so a bit of casting, too.
            accessString = "reinterpret_cast<const LocalStorageEntry*>(reinterpret_cast<const unsigned char*>(localStore) + " + accessString + ")->val." + type.name + "Val";
        }

        mInd(16) << variant.op.implParams[p].type.nativeName << " " << variant.op.implParams[p].name
                     << " = ";
        if (type == variant.op.implParams[p].type) {
            // We don't need a conversion, just fetch it directly into name..
            out(MaCpp) << accessString << ";\n";
        } else {
            ConversionInfo conv = types.immConv(type, variant.op.implParams[p].type);
            out(MaCpp) << "convert" << conv.name << "(exec, " << accessString << ");\n";

            if (conv.flags & Conv_MayThrow) {
                // Check for an exception being raised, or perhaps a reload request
                mInd(16) << "if (pc != localPC) // {// Exception or reload\n";
                //mInd(20) << "if (exec->h
                mInd(20) << "continue;\n";
            }
        }
    }

    // Replace $$ with destination register
    string storeCode = "localStore[fbDestReg].val." + variant.op.retType + "Val";
    string code = variant.op.implementAs;
    code = strReplace(code, "$$", storeCode);

    // Print out the impl code..
    out.printCode(out(MaCpp), 16, code, variant.op.codeLine);
}

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
