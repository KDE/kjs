/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
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

#ifndef STRING_OBJECT_H_
#define STRING_OBJECT_H_

#include "function_object.h"
#include "JSWrapperObject.h"
#include "internal.h"

namespace KJS
{

class StringInstance : public JSWrapperObject
{
public:
    StringInstance(JSObject *proto);
    StringInstance(JSObject *proto, StringImp *);
    StringInstance(JSObject *proto, const UString &);

    bool getOwnPropertySlot(ExecState *, const Identifier &, PropertySlot &) override;
    bool getOwnPropertySlot(ExecState *, unsigned propertyName, PropertySlot &) override;

    using KJS::JSObject::put;
    void put(ExecState *exec, const Identifier &propertyName, JSValue *, int attr = None) override;
    using KJS::JSObject::deleteProperty;
    bool deleteProperty(ExecState *exec, const Identifier &propertyName) override;
    void getOwnPropertyNames(ExecState *, PropertyNameArray &, PropertyMap::PropertyMode mode) override;
    bool getOwnPropertyDescriptor(ExecState *, const Identifier &, PropertyDescriptor &) override;

    UString toString(ExecState *exec) const override;
    JSObject *valueClone(Interpreter *targetCtx) const override;

    const ClassInfo *classInfo() const override
    {
        return &info;
    }
    static const ClassInfo info;

    StringImp *internalValue() const
    {
        return static_cast<StringImp *>(JSWrapperObject::internalValue());
    }

    bool conversionsCustomized() const
    {
        return m_conversionsCustomized;
    }
private:
    bool inlineGetOwnPropertySlot(ExecState *, unsigned, PropertySlot &);

    static JSValue *lengthGetter(ExecState *, JSObject *, const Identifier &, const PropertySlot &);
    static JSValue *indexGetter(ExecState *, JSObject *, unsigned, const PropertySlot &);

    bool m_conversionsCustomized;
};

/**
 * @internal
 *
 * The initial value of String.prototype (and thus all objects created
 * with the String constructor
 */
class StringPrototype : public StringInstance
{
public:
    StringPrototype(ExecState *exec,
                    ObjectPrototype *objProto);
    using KJS::StringInstance::getOwnPropertySlot;
    bool getOwnPropertySlot(ExecState *, const Identifier &, PropertySlot &) override;
    const ClassInfo *classInfo() const override
    {
        return &info;
    }
    static const ClassInfo info;
};

/**
 * @internal
 *
 * Class to implement all methods that are properties of the
 * String.prototype object
 */
class StringProtoFunc : public InternalFunctionImp
{
public:
    StringProtoFunc(ExecState *exec, int i, int len, const Identifier &);

    JSValue *callAsFunction(ExecState *exec, JSObject *thisObj, const List &args) override;

    enum { ToString, ValueOf, CharAt, CharCodeAt, Concat, IndexOf, LastIndexOf,
           Match, Replace, Search, Slice, Split,
           Substr, Substring, FromCharCode, ToLowerCase, ToUpperCase,
           ToLocaleLowerCase, ToLocaleUpperCase, Trim, LocaleCompare,
           StartsWith, EndsWith, Includes,
           // ES6 (Draft 08.11.2013
           Repeat
#ifndef KJS_PURE_ECMA
           , Big, Small, Blink, Bold, Fixed, Italics, Strike, Sub, Sup,
           Fontcolor, Fontsize, Anchor, Link, TrimLeft, TrimRight
#endif
         };

    static void setToLowerFunction(UnicodeSupport::StringConversionFunction f);
    static void setToUpperFunction(UnicodeSupport::StringConversionFunction f);
private:
    int id;
};

/**
 * @internal
 *
 * The initial value of the global variable's "String" property
 */
class StringObjectImp : public InternalFunctionImp
{
public:
    StringObjectImp(ExecState *exec,
                    FunctionPrototype *funcProto,
                    StringPrototype *stringProto);

    bool implementsConstruct() const override;
    using KJS::JSObject::construct;
    JSObject *construct(ExecState *exec, const List &args) override;
    JSValue *callAsFunction(ExecState *exec, JSObject *thisObj, const List &args) override;
};

/**
 * @internal
 *
 * Class to implement all methods that are properties of the
 * String object
 */
class StringObjectFuncImp : public InternalFunctionImp
{
public:
    StringObjectFuncImp(ExecState *, FunctionPrototype *, const Identifier &);
    JSValue *callAsFunction(ExecState *exec, JSObject *thisObj, const List &args) override;
};

} // namespace

#endif

