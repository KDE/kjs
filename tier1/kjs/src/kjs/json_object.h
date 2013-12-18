/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2012 Bernd Buschinski (b.buschinski@googlemail.com)
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

#ifndef JSON_OBJECT_H_
#define JSON_OBJECT_H_

#include "function_object.h"

namespace KJS {

class JSONObjectImp : public JSObject {
public:
    JSONObjectImp(ExecState *exec,
                  ObjectPrototype *objProto);

    using KJS::JSObject::getOwnPropertySlot;
    bool getOwnPropertySlot(ExecState *, const Identifier&, PropertySlot&);

    virtual const ClassInfo *classInfo() const { return &info; }
    static const ClassInfo info;
    enum { Parse, Stringify };
};

class JSONFuncImp : public InternalFunctionImp {
public:
    JSONFuncImp(ExecState *exec, int i, int l, const Identifier&);
    virtual JSValue *callAsFunction(ExecState*, JSObject*, const List&);
private:
    int id;
};

} // namespace

#endif // JSON_OBJECT_H
