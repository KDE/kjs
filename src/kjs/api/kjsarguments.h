/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2008 Harri Porten (porten@kde.org)
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

#ifndef KJSARGUMENTS_H
#define KJSARGUMENTS_H

#include "kjsapi_export.h"
#include "kjsobject.h"

class KJSArgumentsHandle;
class KJSCustomFunction;

/**
 * A class representing a list of JavaScript arguments.
 *
 * @short Argument list
 */
class KJSAPI_EXPORT KJSArguments
{
    friend class KJSCustomFunction;
public:
    /**
     * Returns the number of arguments.
     */
    int count() const;
    /**
     * Returns the argument at the specified index. Accessing an
     * argument outside of the valid range will return an object of
     * type "undefined".
     */
    KJSObject at(int idx) const;
private:
    KJSArguments(const KJSArguments&); // undefined
    KJSArguments& operator=(const KJSArguments&); // undefined

    KJSArguments(const KJSArgumentsHandle* h) : hnd(h) { }
    const KJSArgumentsHandle* hnd;
};

#endif
