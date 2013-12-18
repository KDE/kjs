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

#ifndef KJS_PACKAGE_H
#define KJS_PACKAGE_H

#include "global.h"
#include "identifier.h"
#include "object.h"

namespace KJS {

    class Package;

    class KJS_EXPORT Package
    {
    public:
	Package(Package* p, const Identifier& n) : prnt(p), nm(n) { }
	virtual ~Package() { }

	const Package* parent() const { return prnt; }
	Package* parent() { return prnt; }

	Identifier name() const { return nm; }

	virtual Package* loadSubPackage(const Identifier& n,
					UString* err);
	virtual void loadSymbol(ExecState* exec, JSObject* obj,
				const Identifier& n);
	virtual void loadAllSymbols(ExecState* exec, JSObject* obj);

    private:
	Package* prnt;
	Identifier nm;
    };

    class KJS_EXPORT StandardGlobalPackage : public Package
    {
    public:
	StandardGlobalPackage();
	virtual Package* loadSubPackage(const Identifier& n,
					UString* err);
    };

    class KJS_EXPORT PackageObject : public JSObject
    {
    public:
	PackageObject(Package *p) : pkg(p) { }

	Package* package() { return pkg; }

	virtual const ClassInfo *classInfo() const { return &info; }
	static const ClassInfo info;

    private:
	Package* pkg;
    };
}

#endif

