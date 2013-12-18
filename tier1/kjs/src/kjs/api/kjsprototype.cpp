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

#include "kjsprototype.h"
#include "kjsinterpreter.h"
#include "kjsarguments.h"
#include "kjsprivate.h"

#include "kjs/object.h"
#include "kjs/JSVariableObject.h"
#include "kjs/context.h"
#include "kjs/interpreter.h"

#include <QMap>

using namespace KJS;

class KJSCustomProperty
{
public:
    KJSCustomProperty(KJSPrototype::PropertyGetter g,
                      KJSPrototype::PropertySetter s)
        : getter(g), setter(s)
    {
    }

    JSValue* read(ExecState* exec, void* object);
    void write(ExecState* exec, void* object, JSValue* value);

private:
    KJSPrototype::PropertyGetter getter;
    KJSPrototype::PropertySetter setter;
};

class CustomObjectInfo {
public:
    CustomObjectInfo(void* v): iv(v) {}
    virtual ~CustomObjectInfo() {} 
    void* internalValue() { return iv; }
protected:
    void* iv;
};

template<class Base>
class CustomObject : public Base, public CustomObjectInfo {
public:
    CustomObject(JSValue* proto, void* v)
        : Base(proto),
          CustomObjectInfo(v)
    {}

    using Base::put;
    void put(ExecState* exec, const Identifier& id,
             JSValue *value, int attr = None);


    // rtti
    static const ClassInfo info;
    const ClassInfo* classInfo() const { return &info; }
};

template<>
const ClassInfo CustomObject<JSObject>::info = { "CustomObject", 0, 0, 0 };

template<>
const ClassInfo CustomObject<JSGlobalObject>::info = { "CustomGlobalObject", 0, 0, 0 };

class KJSCustomFunction : public JSObject {
public:
    KJSCustomFunction(ExecState* exec, KJSPrototype::FunctionCall f)
        : callback(f)
    {
        setPrototype(exec->lexicalInterpreter()->builtinObjectPrototype());
    }

    JSValue* callAsFunction(ExecState* exec, JSObject* thisObj,
                            const List &args);
    bool implementsCall() const { return true; }

private:
    KJSPrototype::FunctionCall callback;
};

JSValue* KJSCustomFunction::callAsFunction(ExecState* exec, JSObject* thisObj,
                                           const List &args)
{
    // FIXME: does not protect against mixing custom objects
    CustomObjectInfo* inf = dynamic_cast<CustomObjectInfo*>(thisObj);

    if (!inf) {
        const char* errMsg = "Attempt at calling a function with an invalid receiver";
        KJS::JSObject *err = KJS::Error::create(exec, KJS::TypeError, errMsg);
        exec->setException(err);
        return err;
    }

    void* thisValue = inf->internalValue();

    KJSContext ctx(EXECSTATE_HANDLE(exec));
    KJSArguments a(LIST_HANDLE(&args));
    KJSObject res = (*callback)(&ctx, thisValue, a);
    return JSVALUE(&res);
}

JSValue* KJSCustomProperty::read(ExecState* exec, void* object)
{
    assert(getter);
    
    KJSContext ctx(EXECSTATE_HANDLE(exec));
    KJSObject res = (*getter)(&ctx, object);
    return JSVALUE(&res);
}

void KJSCustomProperty::write(ExecState* exec, void* object, JSValue* value)
{
    KJSContext ctx(EXECSTATE_HANDLE(exec));

    if (setter) {
        KJSObject vo(JSVALUE_HANDLE(value));
        (*setter)(&ctx, object, vo);
    } else {
        JSObject *e = Error::create(exec, GeneralError,
                                    "Property is read-only");
        exec->setException(e);
    }
}

static JSValue* getPropertyValue(ExecState* exec, JSObject *originalObject,
                                 const Identifier&, const PropertySlot& sl)
{
    CustomObjectInfo* inf = dynamic_cast<CustomObjectInfo*>(originalObject);
    if (!inf)
        return jsUndefined();

    KJSCustomProperty* p =
        reinterpret_cast<KJSCustomProperty*>(sl.customValue());

    return p->read(exec, inf->internalValue());
}

// FIXME: or use Identifier?
// FIXME: use values
typedef QMap<UString, KJSCustomProperty*> CustomPropertyMap;

class CustomPrototype : public JSObject {
public:
    CustomPrototype()
    {
    }
    ~CustomPrototype()
    {
        qDeleteAll(properties);
    }

    using KJS::JSObject::getOwnPropertySlot;
    bool getOwnPropertySlot(ExecState *exec, const Identifier& id,
                            PropertySlot& sl)
    {
        CustomPropertyMap::iterator it = properties.find(id.ustring());
        if (it == properties.end())
            return JSObject::getOwnPropertySlot(exec, id, sl);
        
        sl.setCustomValue(0, *it, getPropertyValue);

        return true;
    }

    void registerProperty(const QString& name,
                          KJSPrototype::PropertyGetter g,
                          KJSPrototype::PropertySetter s)
    {
        properties.insert(toUString(name), new KJSCustomProperty(g, s));
    }

    void registerFunction(ExecState* exec,
                          const QString& name, KJSPrototype::FunctionCall f)
    {
        putDirect(toIdentifier(name), new KJSCustomFunction(exec, f));
    }

    template<typename Base>
    bool setProperty(ExecState* exec, CustomObject<Base>* obj,
                     const Identifier& id, JSValue* value)
    {
        CustomPropertyMap::iterator it = properties.find(id.ustring());
        if (it == properties.end())
            return false;

        (*it)->write(exec, obj->internalValue(), value);
        
        return true;
    }

private:
    CustomPropertyMap properties;
};

template<class Base>
void CustomObject<Base>::put(ExecState* exec, const Identifier& id,
                       JSValue* value, int attr)
{
    CustomPrototype* p = static_cast<CustomPrototype*>(this->prototype());

    if (!p->setProperty(exec, this, id, value))
        Base::put(exec, id, value, attr);
}

KJSPrototype::KJSPrototype()
{
    CustomPrototype* p = new CustomPrototype;
    gcProtect(p);

    hnd = PROTOTYPE_HANDLE(p);
}

KJSPrototype::~KJSPrototype()
{
    gcUnprotect(PROTOTYPE(this));
}

void KJSPrototype::defineConstant(const QString& name, double value)
{
    CustomPrototype* p = PROTOTYPE(this);

    p->putDirect(toIdentifier(name), jsNumber(value),
                 DontEnum|DontDelete|ReadOnly);
}

void KJSPrototype::defineConstant(const QString& name, const QString& value)
{
    CustomPrototype* p = PROTOTYPE(this);

    p->putDirect(toIdentifier(name), jsString(toUString(value)),
                 DontEnum|DontDelete|ReadOnly);
}

void KJSPrototype::defineConstant(const QString& name, const KJSObject& value)
{
    CustomPrototype* p = PROTOTYPE(this);

    p->putDirect(toIdentifier(name), JSVALUE(&value),
                 DontEnum|DontDelete|ReadOnly);
}

KJSObject KJSPrototype::constructObject(KJSContext* ctx, void *internalValue)
{
    CustomPrototype* p = PROTOTYPE(this);

    if (ctx && !p->prototype()) {
        ExecState* exec = EXECSTATE(ctx);
        KJS::Interpreter* i = exec->lexicalInterpreter();
        JSObject* objectProto = i->builtinObjectPrototype();
        p->setPrototype(objectProto);
    }

    CustomObject<JSObject>* newObj = new CustomObject<JSObject>(p, internalValue);
    return KJSObject(JSVALUE_HANDLE(newObj));
}

KJSGlobalObject KJSPrototype::constructGlobalObject(void *internalValue)
{
    CustomPrototype* p = PROTOTYPE(this);

    CustomObject<JSGlobalObject>* newObj = new CustomObject<JSGlobalObject>(p, internalValue);
    return KJSGlobalObject(JSVALUE_HANDLE(newObj));
}

void KJSPrototype::defineProperty(KJSContext* ctx,
                                  const QString& name,
                                  PropertyGetter getter,
                                  PropertySetter setter)
{
    Q_UNUSED(ctx);
    assert(getter);

    CustomPrototype* p = PROTOTYPE(this);

    p->registerProperty(name, getter, setter);
}

void KJSPrototype::defineFunction(KJSContext* ctx,
                                  const QString& name, FunctionCall callback)
{
    assert(callback);

    CustomPrototype* p = PROTOTYPE(this);
    ExecState* exec = EXECSTATE(ctx);

    p->registerFunction(exec, name, callback);
}


// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
