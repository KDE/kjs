/*
 *  Opcode argument data structures KJS/Frostbyte
 *  This file is part of the KDE libraries
 *  Copyright (C) 2008 Maksim Orlovich (maksim@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#ifndef OP_ARGS_H
#define OP_ARGS_H

#include "opcodes.h"

namespace KJS {

class Node;
class RegDescriptor;

// The NarrowArg and WideArg unions correspond to the encoding of
// 4- and 8-byte arguments in the bytecode.
// Note that the union field names should correspond to codes.def types ---
// type foo will be accessed as fooVal

union NarrowArg
{
    bool     boolVal;
    int32_t  int32Val;
    Addr     addrVal;
    Register regVal;
    unsigned char asBytes[4];
};

union WideArg
{
    JSValue*    valueVal;
    Identifier* identVal;
    UString*    stringVal;
    double      numberVal;
    Node*       nodeVal;
    const char* cstrVal;
    unsigned char asBytes[8];
};

// This describes where result of evaluating an expression, or an argument
// to a function, is stored. If immediate is true, it is included directly inside
// the structure. Otherwise, it's passed by the register number specified in
// ownedReg->reg()
// Note: there is a difference between values stored in registers, and register names.
// The former have immediate = false, and some type such as OpType_Bool; the latter have
// immediate = true, and type = OpType_reg.
struct OpValue
{
    bool   immediate;
    OpType type;
    RefPtr<RegDescriptor> ownedReg; // Register the lifetime of which we reserve, including the register #
    union {
        NarrowArg  narrow;
        WideArg    wide;
    } value;

    OpValue();

    // A few helpers for making immediate values... These are actually inside CompileState.h
    // due to them touching the ownedTemp;

    static void initImm(OpValue* val, OpType type) {
        val->immediate = true;
        val->type      = type;
    }

    // A placeholder address value, will be patched up later
    static OpValue* dummyAddr();

    static OpValue immInt32(int32_t in);
    static OpValue immNumber(double in);
    static OpValue immValue(JSValue* in);
    static OpValue immBool(bool in);
    static OpValue immString(UString* in);
    static OpValue immIdent(Identifier* in);
    static OpValue immNode(Node* in);
    static OpValue immCStr(const char* in);
    static OpValue immAddr(Addr in);
};

}

#endif

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
