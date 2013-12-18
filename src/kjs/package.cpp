/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2006-2007 Harri Porten (porten@kde.org)
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

#include "package.h"

using namespace KJS;

const ClassInfo PackageObject::info = { "Package", 0, 0, 0 };

Package* Package::loadSubPackage(const Identifier&, UString* /*err*/)
{
    return 0;
}

void Package::loadSymbol(ExecState*, JSObject*, const Identifier&)
{
}

void Package::loadAllSymbols(ExecState*, JSObject*)
{
}

StandardGlobalPackage::StandardGlobalPackage()
    : Package(0, 0)
{
}

Package* StandardGlobalPackage::loadSubPackage(const Identifier& n,
					       UString* err)
{
    return Package::loadSubPackage(n, err);
}

