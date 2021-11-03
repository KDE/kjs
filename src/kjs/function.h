/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2003, 2004, 2005, 2006, 2007 Apple Inc. All rights reserved.
 *  Copyright (C) 2007, 2008 Maksim Orlovich <maksim@kde.org>
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

#ifndef KJS_FUNCTION_H
#define KJS_FUNCTION_H

#include "object.h"
#include "JSVariableObject.h"
#include <wtf/OwnPtr.h>

namespace KJS
{

class ActivationImp;
class FunctionPrototype;

class KJS_EXPORT InternalFunctionImp : public JSObject
{
public:
    InternalFunctionImp();
    InternalFunctionImp(FunctionPrototype *);
    InternalFunctionImp(FunctionPrototype *, const Identifier &);

    bool implementsCall() const override;
    JSValue *callAsFunction(ExecState *, JSObject *thisObjec, const List &args) override = 0;
    bool implementsHasInstance() const override;

    const ClassInfo *classInfo() const override
    {
        return &info;
    }
    static const ClassInfo info;
    const Identifier &functionName() const
    {
        return m_name;
    }
    void setFunctionName(const Identifier &name)
    {
        m_name = name;
    }

private:
    Identifier m_name;
#ifdef _WIN32
    InternalFunctionImp(const InternalFunctionImp &);
    InternalFunctionImp &operator=(const InternalFunctionImp &);
#endif
};

/**
 * A minimal object that just throws an exception if executed.
 */
class Thrower : public JSObject
{
public:
    Thrower(ErrorType type);

    JSValue *callAsFunction(ExecState *exec, JSObject *, const List &args) override;
    bool implementsCall() const override
    {
        return true;
    };
private:
    ErrorType m_type;
};

class BoundFunction : public InternalFunctionImp
{
public:
    explicit BoundFunction(ExecState *exec, JSObject *targetFunction, JSObject *boundThis, List boundArgs);

    void setTargetFunction(JSObject *targetFunction);
    void setBoundThis(JSObject *boundThis);
    void setBoundArgs(const List &boundArgs);

    JSValue *callAsFunction(ExecState *exec, JSObject *thisObj, const List &extraArgs) override;
    bool implementsCall() const override
    {
        return true;
    };

    using KJS::JSObject::construct;
    JSObject *construct(ExecState *exec, const List &extraArgs) override;
    bool implementsConstruct() const override
    {
        return true;
    };

    bool hasInstance(ExecState *exec, JSValue *value) override;
    bool implementsHasInstance() const override
    {
        return true;
    };

private:
    ProtectedPtr<JSObject> m_targetFunction;
    ProtectedPtr<JSObject> m_boundThis;
    List m_boundArgs;
};

/**
 * @internal
 *
 * The initial value of Function.prototype (and thus all objects created
 * with the Function constructor)
 */
class KJS_EXPORT FunctionPrototype : public InternalFunctionImp
{
public:
    FunctionPrototype(ExecState *exec);
    ~FunctionPrototype() override;

    JSValue *callAsFunction(ExecState *exec, JSObject *thisObj, const List &args) override;
};

class IndexToNameMap
{
public:
    IndexToNameMap(FunctionImp *func, const List &args);
    ~IndexToNameMap();

    Identifier &operator[](int index);
    Identifier &operator[](const Identifier &indexIdentifier);
    bool isMapped(const Identifier &index) const;
    void unMap(const Identifier &index);
    int size() const;

private:
    IndexToNameMap(); // prevent construction w/o parameters
    int _size;
    Identifier *_map;
};

class Arguments : public JSObject
{
public:
    Arguments(ExecState *exec, FunctionImp *func, const List &args, ActivationImp *act);
    void mark() override;
    using KJS::JSObject::getOwnPropertySlot;
    bool getOwnPropertySlot(ExecState *, const Identifier &, PropertySlot &) override;
    using KJS::JSObject::put;
    void put(ExecState *exec, const Identifier &propertyName, JSValue *value, int attr = None) override;
    using KJS::JSObject::deleteProperty;
    bool deleteProperty(ExecState *exec, const Identifier &propertyName) override;
    void getOwnPropertyNames(ExecState *, PropertyNameArray &, PropertyMap::PropertyMode mode) override;

    bool defineOwnProperty(ExecState *exec, const Identifier &propertyName, PropertyDescriptor &desc, bool shouldThrow) override;

    const ClassInfo *classInfo() const override
    {
        return &info;
    }
    static const ClassInfo info;
private:
    static JSValue *mappedIndexGetter(ExecState *exec, JSObject *, const Identifier &, const PropertySlot &slot);

    ActivationImp *_activationObject;
    mutable IndexToNameMap indexToNameMap;
};

class ActivationImp : public JSVariableObject
{
public:
    enum {
        FunctionSlot        = NumVarObjectSlots,
        ArgumentsObjectSlot,
        NumReservedSlots = ArgumentsObjectSlot + 1
    };

    void setup(ExecState *exec, FunctionImp *function, const List *arguments,
               LocalStorageEntry *stackSpace);

    // Request that this activation be torn off when the code using it stops running
    void requestTearOff();
    void performTearOff();

    using KJS::JSObject::getOwnPropertySlot;
    bool getOwnPropertySlot(ExecState *exec, const Identifier &, PropertySlot &) override;
    using KJS::JSObject::put;
    void put(ExecState *exec, const Identifier &propertyName, JSValue *value, int attr = None) override;
    using KJS::JSObject::deleteProperty;
    bool deleteProperty(ExecState *exec, const Identifier &propertyName) override;

    void putDirect(const Identifier &propertyName, JSValue *value, int attr = 0) override;
    using JSObject::putDirect;
    JSValue *getDirect(const Identifier &propertyName) const override;
    bool getPropertyAttributes(const Identifier &propertyName, unsigned &attributes) const override;

    bool isLocalReadOnly(int propertyID) const
    {
        return (localStorage[propertyID].attributes & ReadOnly) == ReadOnly;
    }

    const ClassInfo *classInfo() const override
    {
        return &info;
    }
    static const ClassInfo info;

    bool isActivation() const override
    {
        return true;
    }
    void setupLocals(FunctionBodyNode *fbody);
    void setupFunctionLocals(FunctionBodyNode *fbody, ExecState *exec);

    const List &passedInArguments() const
    {
        return *arguments;
    }

    // really FunctionImp, but type isn't declared yet
    JSValue *function()
    {
        return functionSlot();
    }
private:
    JSValue *&functionSlot()
    {
        return localStorage[FunctionSlot].val.valueVal;
    }

    JSValue *&argumentsObjectSlot()
    {
        return localStorage[ArgumentsObjectSlot].val.valueVal;
    }

    static PropertySlot::GetValueFunc getArgumentsGetter();
    static JSValue *argumentsGetter(ExecState *exec, JSObject *, const Identifier &, const PropertySlot &slot);
    void createArgumentsObject(ExecState *exec);

    int  numLocals() const
    {
        return lengthSlot();
    }
    bool validLocal(int id) const
    {
        return 0 <= id && id < numLocals();
    }
    const List *arguments;
};

class GlobalFuncImp : public InternalFunctionImp
{
public:
    GlobalFuncImp(ExecState *, FunctionPrototype *, int i, int len, const Identifier &);
    JSValue *callAsFunction(ExecState *exec, JSObject *thisObj, const List &args) override;
    enum { Eval, ParseInt, ParseFloat, IsNaN, IsFinite, Escape, UnEscape,
           DecodeURI, DecodeURIComponent, EncodeURI, EncodeURIComponent
#ifndef NDEBUG
           , KJSPrint
#endif
         };
private:
    int id;
};

static const double mantissaOverflowLowerBound = 9007199254740992.0;
double parseIntOverflow(const char *s, int length, int radix);

double parseInt(const UString &s, int radix);
double parseFloat(const UString &s);

} // namespace

#endif
