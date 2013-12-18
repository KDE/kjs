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

#ifndef KJSPROTOTYPE_H
#define KJSPROTOTYPE_H

#include "kjsapi_export.h"
#include "kjscontext.h"

class KJSInterpreter;
class KJSArguments;
class KJSGlobalObject;
class KJSPrototypeHandle;

/**
 * A class representing a JavaScript prototype object.
 *
 * @short Prototype object.
 */
class KJSAPI_EXPORT KJSPrototype
{
public:
    /**
     * Constructs a prototype object with its own prototype property
     * set to the Object prototype.
     */
    KJSPrototype();
    /**
     * Destructs this prototype object.
     */
    ~KJSPrototype();
    /**
     * Add a read-only numerical property to this object.
     */
    void defineConstant(const QString& name, double value);
    /**
     * Add a read-only string property to this object.
     *
     * @overload
     */
    void defineConstant(const QString& name, const QString& value);
    /**
     * Add a read-only object property to this object.
     *
     * @overload
     */
    void defineConstant(const QString& name, const KJSObject& value);
    /**
     * Function signature for a property getter function. Describes
     * one of the defineProperty() argument types.
     */
    typedef KJSObject (*PropertyGetter)(KJSContext* context, void* object);
    /**
     * Function signature for a property setter function. Describes
     * one of the defineProperty() argument types.
     */
    typedef void (*PropertySetter)(KJSContext* context, void* object,
                                   KJSObject value);
    /**
     * Defines a property of this prototype with C++ callback
     * functions for getting and setting the property value. If the
     * setter is set to 0 then the property is treated as read-only.
     */
    void defineProperty(KJSContext* ctx,
                        const QString& name,
                        PropertyGetter getter,
                        PropertySetter setter = 0);

    /**
     * Signature for function call callback function. The
     * defineFunction() function parameter accepts a reference to a
     * function of this type as a parameter.
     */
    typedef KJSObject (*FunctionCall)(KJSContext* context, void* object,
                                      const KJSArguments& arguments);

    /**
     * Define a function. The specified C++ callback function will
     * be called upon invocation.
     */
    void defineFunction(KJSContext* ctx,
                        const QString& name, FunctionCall callback);

#if 0
    /**
     * Define a finalization function.
     *
     */
    void defineFinalization(FinalizeFunction callback);
#endif

    /**
     * Construct an object with this prototype and the specified
     * internal value. The value pointer will be passed as the object
     * parameter to the callbacks defined via defineProperty() and
     * defineFunction().
     *
     * @todo Provide a better type than void*
     */
    KJSObject constructObject(KJSContext* ctx, void *internalValue = 0);

    /**
     * Similar to constructObject() but specialized on the
     * construction of global objects.
     */
    KJSGlobalObject constructGlobalObject(void *internalValue = 0);

private:
    KJSPrototype(const KJSPrototype&); // undefined
    KJSPrototype& operator=(const KJSPrototype&); // undefined

    KJSPrototypeHandle* hnd;
};

#endif
