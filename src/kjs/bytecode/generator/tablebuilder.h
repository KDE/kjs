/*
 *  A utility for building various tables and specializations for the
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

#ifndef TABLE_BUILDER_H
#define TABLE_BUILDER_H

#include "filetemplate.h"
#include "codeprinter.h"
#include "types.h"
#include "parser.h"

// Actually, a specialization, but who cares?
struct Operation {
    string name;
    string retType;
    int          cost;
    int          codeLine;
    unsigned     flags;
    bool         isTile;

    string implementAs;
    vector<Type>      opParamTypes; // the types of params of the bytecode operation
    vector<Parameter> implParams;   // the params the [[ code ]]  block takes
};

struct OperationVariant {
    string   sig;
    Operation op;
    vector<bool> paramIsIm;
    vector<int>  paramOffsets;
    int         size;
    bool        needsPadVariant;
};

class TableBuilder: public Parser
{
public:
    TableBuilder(istream *inStream, ostream *hStream, ostream *cppStream,
                 FileTemplate *fTemplate, ostream *mStream);

    ~TableBuilder() override {}
    void generateCode();
private:
    // Interface to the parser; also (ab)used by the type system to emit
    // conversion ops.
    friend class TypeTable;

    void handleType(const string &type, const string &nativeName, unsigned flags) override;
    void handleConversion(const string &runtimeRoutine, int codeLine,
                                  unsigned flags, const string &from, const string &to,
                                  int tileCost, int registerCost) override;

    void handleOperation(const string &name, unsigned flags) override;
    void handleImpl(const string &fnName, const string &code, int codeLine, int cost,
                            const string &retType, vector<Parameter> sig) override;
    void handleTile(const string &fnName, StringList sig) override;

    // Enumerates all r/i/pad variants; plus computes the shuffle table.
    void expandOperationVariants(const Operation &op, vector<bool> &paramIsIm);

    void dumpOpStructForVariant(const OperationVariant &variant, bool doPad,
                                bool hasPadVariant, bool needsComma);

    void generateVariantImpl(const OperationVariant &variant);

    CodePrinter  out;
    TypeTable    types;

    ostream &mInd(int ind)
    {
        return out.mInd(ind);
    }

    StringList          operationNames;
    unsigned            operationFlags;
    map<string, string> operationRetTypes; // ugly enough specified on the impl. I suck.
    vector<Operation>   operations;
    map<string, Operation> implementations;

    StringList  variantNames;
    vector<OperationVariant> variants;
    map<string, StringList>  variantNamesForOp;

    FileTemplate *fTemplate;
};

#endif
