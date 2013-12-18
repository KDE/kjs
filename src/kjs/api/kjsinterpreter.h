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

#ifndef KJSINTERPRETER_H
#define KJSINTERPRETER_H

#include "kjsapi_export.h"
#include "kjsobject.h"
#include "kjscontext.h"

class KJSPrototype;
class KJSInterpreter;
class KJSInterpreterHandle;
class KJSResultHandle;

/**
 * A class representing the result of a script evaluation.
 */
class KJSAPI_EXPORT KJSResult
{
    friend class KJSInterpreter;
public:
    /**
     * Constructs a default result object.
     */
    KJSResult();
    /**
     * Constructs a copy of another result object.
     */
    KJSResult(const KJSResult&);
    /**
     * Assigns the properties of another result object to this one.
     */
    KJSResult& operator=(const KJSResult&);
    /**
     * Frees resources held by this result object.
     */
    ~KJSResult();
    /**
     * Returns true if the script evaluation has caused an exception.
     */
    bool isException() const;

    /**
     * Returns the error message if this is an exception result.
     */
    QString errorMessage() const;
    /*
     * If the evaluation was successful, i.e. isException() is false
     * this function returns the value returned by the script. Can be
     * an "undefined" (isUndefined()) value.
     */
    KJSObject value() const;

private:
    KJSResultHandle* hnd;
};

/**
 * A class representing a JavaScript interpreter
 *
 * @short JavaScript interpreter
 */
class KJSAPI_EXPORT KJSInterpreter
{
    friend class KJSResult;
    friend class KJSPrototype;
    friend class KJSContext;
public:
    /**
     * Constructs an interpreter with a default global object.
     */
    KJSInterpreter();
    /**
     * Constructs an interpreter with a custom global object.
     */
    KJSInterpreter(const KJSGlobalObject& global);
    /**
     * Creates a copy of another interpreter.
     */
    KJSInterpreter(const KJSInterpreter& other);
    /**
     * Assign another interpreter instance to this object.
     */
    KJSInterpreter& operator=(const KJSInterpreter& other);
    /**
     * Destructs this interpreter and frees resources it has
     * allocated. This renders any still existing objects referencing
     * those invalid.
     */
    ~KJSInterpreter();

    /**
     * Returns a handle to the global execution context.
     */
    KJSContext* globalContext();
    /**
     * @overload
     */
    const KJSContext* globalContext() const;
    /**
     * Returns the object that is used as the global object during all
     * script execution performed by this interpreter,
     */
    KJSObject globalObject();
    /**
     * Evaluates a piece of code with a "this" set to (optionally set)
     * value. The sourceURL and startingLineNumber parameters are used
     * to provide information about the origin of a parse error or
     * runtime exception.
     */
    KJSResult evaluate(const QString& sourceURL, int startingLineNumber,
                       const QString& code,
                       KJSObject* thisValue = 0);
    /**
     *  @overload
     */
    KJSResult evaluate(const QString& code,
                       KJSObject* thisValue = 0);

    /**
     * Reformat the given script code to an easy to read format with
     * only one statement per line. This can be useful when debugging
     * a script that was e.g. condensed into a single line to a single
     * line. While comments will be removed the script will remain
     * unchanged semantically.
     *
     * @param codeIn The code to be reformatted
     * @param codeOut Points to string holding the result.
     * @param errLine Will hold the line of a parse error
     * @param errMsg Will hold the message of a parse error
     *
     * @return Returns true if the reformatting was successful, false
     * on a parse error.
     */
    static bool normalizeCode(const QString& codeIn, QString* codeOut,
                              int* errLine = 0, QString* errMsg = 0);
private:
    KJSInterpreter(KJSInterpreterHandle* h);
    KJSInterpreterHandle* hnd;
    KJSContext globCtx;
};

#endif
