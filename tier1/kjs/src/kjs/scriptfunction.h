// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2003, 2006 Apple Computer, Inc.
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

#ifndef KJS_SCRIPTFUNCTION_H
#define KJS_SCRIPTFUNCTION_H

#include "function.h"

namespace KJS {

  class ActivationImp;
  class FunctionBodyNode;

  /**
   * @short Implementation class for internal Functions.
   */
  class KJS_EXPORT FunctionImp : public InternalFunctionImp {
    friend class ActivationImp;
  public:
    FunctionImp(ExecState* exec, const Identifier& n, FunctionBodyNode* b, const ScopeChain &sc);
    virtual ~FunctionImp();

    using KJS::JSObject::getOwnPropertySlot;
    virtual bool getOwnPropertySlot(ExecState *, const Identifier &, PropertySlot&);
    virtual bool getOwnPropertyDescriptor(ExecState*, const Identifier&, PropertyDescriptor&);
    using KJS::JSObject::put;
    virtual void put(ExecState *exec, const Identifier &propertyName, JSValue *value, int attr = None);
    using KJS::JSObject::deleteProperty;
    virtual bool deleteProperty(ExecState *exec, const Identifier &propertyName);

    virtual JSValue *callAsFunction(ExecState *exec, JSObject *thisObj, const List &args);

    bool implementsConstruct() const;
    using KJS::JSObject::construct;
    JSObject *construct(ExecState *exec, const List &args);

    // Note: implemented in nodes2string.cpp
    UString toSource() const;

    // Note: unlike body->paramName, this returns Identifier::null for parameters
    // that will never get set, due to later param having the same name
    Identifier getParameterName(size_t index);

    virtual const ClassInfo *classInfo() const { return &info; }
    static const ClassInfo info;

    RefPtr<FunctionBodyNode> body;

    /**
     * Returns the scope of this object. This is used when execution declared
     * functions - the execution context for the function is initialized with
     * extra object in its scope. An example of this is functions declared
     * inside other functions:
     *
     * \code
     * function f() {
     *
     *   function b() {
     *     return prototype;
     *   }
     *
     *   var x = 4;
     *   // do some stuff
     * }
     * f.prototype = new String();
     * \endcode
     *
     * When the function f.b is executed, its scope will include properties of
     * f. So in the example above the return value of f.b() would be the new
     * String object that was assigned to f.prototype.
     *
     * @param exec The current execution state
     * @return The function's scope
     */
    const ScopeChain &scope() const { return _scope; }
    void setScope(const ScopeChain &s) { _scope = s; }

    virtual void mark();
  private:
    void initialCompile(ExecState* newExec);
  
    ScopeChain _scope;

    static JSValue *argumentsGetter(ExecState *, JSObject *, const Identifier &, const PropertySlot&);
    static JSValue *callerGetter(ExecState *, JSObject *, const Identifier &, const PropertySlot&);
    static JSValue *lengthGetter(ExecState *, JSObject *, const Identifier &, const PropertySlot&);
    static JSValue *nameGetter(ExecState *, JSObject *, const Identifier &, const PropertySlot&);

    void passInParameters(ExecState *exec, const List &);
  };

  // For compatibility...
  typedef FunctionImp DeclaredFunctionImp;
} // namespace

#endif
