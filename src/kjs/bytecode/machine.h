/*
 *  Main VM dispatch loop and related routines for KJS/Frostbyte
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

#ifndef KJS_MACHINE_H
#define KJS_MACHINE_H

#include "completion.h"
#include "ExecState.h"
#include "value.h"

#include "nodes.h"
#include "opcodes.h"

namespace KJS {

class Machine {
public:
    static JSValue* runBlock(ExecState* exec, const CodeBlock& block, ExecState* parentExec = 0);
};

}

#endif
// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on; hl c++;
