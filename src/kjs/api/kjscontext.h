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

#ifndef KJSCONTEXT_H
#define KJSCONTEXT_H

#include "kjsapi_export.h"

#include <QString>

class KJSObject;
class KJSArray;
class KJSInterpreter;
class KJSContextHandle;
class KJSCustomProperty;
class KJSCustomFunction;

/**
 * A class representing a JavaScript execution context.
 *
 * @short Execution context
 */
class KJSAPI_EXPORT KJSContext
{
    friend class KJSObject;
    friend class KJSArray;
    friend class KJSPrototype;
    friend class KJSInterpreter;
    friend class KJSCustomProperty;
    friend class KJSCustomFunction;
public:
    /**
     * Returns true if this context has an unhandled exception.
     */
    bool hasException() const;
    /**
     * Throws a general exception with the specified error message.
     */
    KJSObject throwException(const QString &message) const;
    /**
     * Returns the currently executing interpreter.
     */
    KJSInterpreter interpreter();
private:
    KJSContext(KJSContextHandle *h);
    KJSContext(const KJSContext &); // undefined
    KJSContext &operator=(const KJSContext &); // undefined

    KJSContextHandle *hnd;
};

#endif
