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
#include "filetemplate.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: icemaker <path>\n";
    }

    std::string path = std::string(argv[1]) + "/"; // krazy:exclude=doublequote_chars

    ifstream def;
    def.open((path + "codes.def").c_str());
    if (def.fail()) {
        std::cerr << "Unable to open codes.def\n";
        return -1;
    }

    FileTemplate opcodesH  (path + "opcodes.h.in",   "opcodes.h");
    FileTemplate opcodesCpp(path + "opcodes.cpp.in", "opcodes.cpp");
    FileTemplate machineCpp(path + "machine.cpp.in", "machine.cpp");

    if (!opcodesH.ok() || !opcodesCpp.ok() || !machineCpp.ok())
        return -1;

    std::cout << "icemaker -41.9 for KJS/FrostByte\n";
    std::cout << "Generating bytecode instruction selection tables and VM dispatcher...\n";

    TableBuilder build(&def, &opcodesH.out, &opcodesCpp.out, &machineCpp, &machineCpp.out);
    build.generateCode();
    return 0;
}

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
