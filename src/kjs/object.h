// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003, 2004, 2005, 2006 Apple Computer, Inc.
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

#ifndef KJS_OBJECT_H
#define KJS_OBJECT_H

#include "global.h"
#include "JSType.h"
#include "interpreter.h"
#include "property_map.h"
#include "property_slot.h"
#include "scope_chain.h"
#include <wtf/AlwaysInline.h>
#include "propertydescriptor.h"

namespace KJS {

  struct HashTable;
  struct HashEntry;
  struct ListImp;
  class InternalFunctionImp;
  class PropertyNameArray;

  /**
   * Class Information
   */
  struct ClassInfo {
    /**
     * A string denoting the class name. Example: "Window".
     */
    const char* className;
    /**
     * Pointer to the class information of the base class.
     * 0L if there is none.
     */
    const ClassInfo *parentClass;
    /**
     * Static hash-table of properties.
     */
    const HashTable *propHashTable;
    /**
     * Reserved for future extension.
     */
    void *dummy;
  };

  // This is an internal value object which stores getter and setter functions
  // for a property.
  class GetterSetterImp : public JSCell {
  public:
    JSType type() const { return GetterSetterType; }

    GetterSetterImp() : getter(0), setter(0) { }

    virtual JSValue *toPrimitive(ExecState *exec, JSType preferred = UnspecifiedType) const;
    virtual bool getPrimitiveNumber(ExecState*, double& number, JSValue*& value);
    virtual bool toBoolean(ExecState *exec) const;
    virtual double toNumber(ExecState *exec) const;
    virtual UString toString(ExecState *exec) const;
    virtual JSObject *toObject(ExecState *exec) const;

    virtual void mark();

    JSObject *getGetter() { return getter; }
    void setGetter(JSObject *g) { getter = g; }
    JSObject *getSetter() { return setter; }
    void setSetter(JSObject *s) { setter = s; }

  private:
    JSObject *getter;
    JSObject *setter;
  };

  class KJS_EXPORT JSObject : public JSCell {
  public:
    /**
     * Creates a new JSObject with the specified prototype
     *
     * @param proto The prototype
     */
    explicit JSObject(JSValue* proto);

    /**
     * Creates a new JSObject with a prototype of jsNull()
     * (that is, the ECMAScript "null" value, not a null object pointer).
     */
    explicit JSObject();

    virtual void mark();
    virtual JSType type() const;

    /**
     * A pointer to a ClassInfo struct for this class. This provides a basic
     * facility for run-time type information, and can be used to check an
     * object's class an inheritance (see inherits()). This should
     * always return a statically declared pointer, or 0 to indicate that
     * there is no class information.
     *
     * This is primarily useful if you have application-defined classes that you
     * wish to check against for casting purposes.
     *
     * For example, to specify the class info for classes FooImp and BarImp,
     * where FooImp inherits from BarImp, you would add the following in your
     * class declarations:
     *
     * \code
     *   class BarImp : public JSObject {
     *     virtual const ClassInfo *classInfo() const { return &info; }
     *     static const ClassInfo info;
     *     // ...
     *   };
     *
     *   class FooImp : public JSObject {
     *     virtual const ClassInfo *classInfo() const { return &info; }
     *     static const ClassInfo info;
     *     // ...
     *   };
     * \endcode
     *
     * And in your source file:
     *
     * \code
     *   const ClassInfo BarImp::info = {"Bar", 0, 0, 0}; // no parent class
     *   const ClassInfo FooImp::info = {"Foo", &BarImp::info, 0, 0};
     * \endcode
     *
     * @see inherits()
     */
    virtual const ClassInfo *classInfo() const;

    /**
     * Checks whether this object inherits from the class with the specified
     * classInfo() pointer. This requires that both this class and the other
     * class return a non-NULL pointer for their classInfo() methods (otherwise
     * it will return false).
     *
     * For example, for two JSObject pointers obj1 and obj2, you can check
     * if obj1's class inherits from obj2's class using the following:
     *
     *   if (obj1->inherits(obj2->classInfo())) {
     *     // ...
     *   }
     *
     * If you have a handle to a statically declared ClassInfo, such as in the
     * classInfo() example, you can check for inheritance without needing
     * an instance of the other class:
     *
     *   if (obj1->inherits(FooImp::info)) {
     *     // ...
     *   }
     *
     * @param cinfo The ClassInfo pointer for the class you want to check
     * inheritance against.
     * @return true if this object's class inherits from class with the
     * ClassInfo pointer specified in cinfo
     */
    bool inherits(const ClassInfo *cinfo) const;

    // internal properties (ECMA 262-3 8.6.2)

    /**
     * Returns the prototype of this object. Note that this is not the same as
     * the "prototype" property.
     *
     * See ECMA 8.6.2
     *
     * @return The object's prototype
     */
    JSValue *prototype() const;
    void setPrototype(JSValue *proto);

    /**
     * Returns the class name of the object
     *
     * See ECMA 8.6.2
     *
     * @return The object's class name
     */
    /**
     * Implementation of the [[Class]] internal property (implemented by all
     * Objects)
     *
     * The default implementation uses classInfo().
     * You should either implement classInfo(), or
     * if you simply need a classname, you can reimplement className()
     * instead.
     */
    virtual UString className() const;

    /**
     * Retrieves the specified property from the object. If neither the object
     * or any other object in its prototype chain have the property, this
     * function will return Undefined.
     *
     * See ECMA 8.6.2.1
     *
     * @param exec The current execution state
     * @param propertyName The name of the property to retrieve
     *
     * @return The specified property, or Undefined
     */
    JSValue *get(ExecState *exec, const Identifier &propertyName) const;
    JSValue *get(ExecState *exec, unsigned propertyName) const;

    bool getPropertySlot(ExecState *, const Identifier&, PropertySlot&);
    bool getPropertySlot(ExecState *, unsigned, PropertySlot&);
    // Fills the PropertyDescriptor looking the ownPropertys and all prototypes until found.
    bool getPropertyDescriptor(ExecState*, const Identifier& propertyName, PropertyDescriptor&);

    virtual bool getOwnPropertySlot(ExecState *, const Identifier&, PropertySlot&);
    virtual bool getOwnPropertySlot(ExecState *, unsigned index, PropertySlot&);
    virtual bool getOwnPropertyDescriptor(ExecState*, const Identifier&, PropertyDescriptor&);

    /**
     * Sets the specified property.
     *
     * See ECMA 8.6.2.2
     *
     * @param exec The current execution state
     * @param propertyName The name of the property to set
     * @param value The value to set
     * @param attr The attributes of the property
     */
    virtual void put(ExecState *exec, const Identifier &propertyName, JSValue *value, int attr = None);
    virtual void put(ExecState *exec, unsigned propertyName, JSValue *value, int attr = None);

    /**
     * Used to check whether or not a particular property is allowed to be set
     * on an object
     *
     * See ECMA 8.6.2.3
     *
     * @param exec The current execution state
     * @param propertyName The name of the property
     * @return true if the property can be set, otherwise false
     */
    /**
     * Implementation of the [[CanPut]] internal property (implemented by all
     * Objects)
     */
    virtual bool canPut(ExecState *exec, const Identifier &propertyName) const;

    /**
     * Checks if a property is enumerable, that is if it doesn't have the DontEnum
     * flag set
     *
     * See ECMA 15.2.4
     * @param exec The current execution state
     * @param propertyName The name of the property
     * @return true if the property is enumerable, otherwise false
     */
    bool propertyIsEnumerable(ExecState *exec, const Identifier &propertyName) const;

    /**
     * Checks to see whether the object (or any object in its prototype chain)
     * has a property with the specified name.
     *
     * See ECMA 8.6.2.4
     *
     * @param exec The current execution state
     * @param propertyName The name of the property to check for
     * @return true if the object has the property, otherwise false
     */
    bool hasProperty(ExecState *exec, const Identifier &propertyName) const;
    bool hasProperty(ExecState *exec, unsigned propertyName) const;

    /**
     * Removes the specified property from the object.
     *
     * See ECMA 8.6.2.5
     *
     * @param exec The current execution state
     * @param propertyName The name of the property to delete
     * @return true if the property was successfully deleted or did not
     * exist on the object. false if deleting the specified property is not
     * allowed.
     */
    virtual bool deleteProperty(ExecState *exec, const Identifier &propertyName);
    virtual bool deleteProperty(ExecState *exec, unsigned propertyName);

    /**
     * Converts the object into a primitive value. The value return may differ
     * depending on the supplied hint
     *
     * See ECMA 8.6.2.6
     *
     * @param exec The current execution state
     * @param hint The desired primitive type to convert to
     * @return A primitive value converted from the objetc. Note that the
     * type of primitive value returned may not be the same as the requested
     * hint.
     */
    /**
     * Implementation of the [[DefaultValue]] internal property (implemented by
     * all Objects)
     */
    virtual JSValue *defaultValue(ExecState *exec, JSType hint) const;

    /**
     * Whether or not the object implements the construct() method. If this
     * returns false you should not call the construct() method on this
     * object (typically, an assertion will fail to indicate this).
     *
     * @return true if this object implements the construct() method, otherwise
     * false
     */
    virtual bool implementsConstruct() const;

    /**
     * Creates a new object based on this object. Typically this means the
     * following:
     * 1. A new object is created
     * 2. The prototype of the new object is set to the value of this object's
     *    "prototype" property
     * 3. The call() method of this object is called, with the new object
     *    passed as the this value
     * 4. The new object is returned
     *
     * In some cases, Host objects may differ from these semantics, although
     * this is discouraged.
     *
     * If an error occurs during construction, the execution state's exception
     * will be set. This can be tested for with ExecState::hadException().
     * Under some circumstances, the exception object may also be returned.
     *
     * Note: This function should not be called if implementsConstruct() returns
     * false, in which case it will result in an assertion failure.
     *
     * @param exec The current execution state
     * @param args The arguments to be passed to call() once the new object has
     * been created
     * @return The newly created &amp; initialized object
     */
    /**
     * Implementation of the [[Construct]] internal property
     */
    virtual JSObject* construct(ExecState* exec, const List& args);
    virtual JSObject* construct(ExecState* exec, const List& args, const Identifier& functionName, const UString& sourceURL, int lineNumber);


    /**
     * If this object represents a value, e.g. is a wrapper around a primitive,
     * a regexp or a date this will return a fresh object with the same value
     * (without cloning properties). Otherwise, returns 0
     *
     * The returned objects will use default prototypes from targetCtx
     */
    virtual JSObject* valueClone(Interpreter* targetCtx) const;

    /**
     * Whether or not this object should be considered a function for the purpose
     * of the typeof operator. Normally this is the same as implementsCall(),
     * which is what the default implementation delegates too, 
     * but in some cases compatibility dictates that the object both be callable
     * and call itself an object and not a function. In this case, this method should
     * be overridden as well
    */
    virtual bool isFunctionType() const;

    /**
     * Calls this object as if it is a function.
     *
     * Note: This function should not be called if implementsCall() returns
     * false, in which case it will result in an assertion failure.
     *
     * See ECMA 8.6.2.3
     *
     * @param exec The current execution state
     * @param thisObj The obj to be used as "this" within function execution.
     * Note that in most cases this will be different from the C++ "this"
     * object. For example, if the ECMAScript code "window.location->toString()"
     * is executed, call() will be invoked on the C++ object which implements
     * the toString method, with the thisObj being window.location
     * @param args List of arguments to be passed to the function
     * @return The return value from the function
     */
    JSValue *call(ExecState *exec, JSObject *thisObj, const List &args); // ### TODO: consolidate with below
    virtual JSValue *callAsFunction(ExecState *exec, JSObject *thisObj, const List &args);

    /**
     * Whether or not the object implements the hasInstance() method. If this
     * returns false you should not call the hasInstance() method on this
     * object (typically, an assertion will fail to indicate this).
     *
     * @return true if this object implements the hasInstance() method,
     * otherwise false
     */
    virtual bool implementsHasInstance() const;

    /**
     * Checks whether value delegates behavior to this object. Used by the
     * instanceof operator.
     *
     * @param exec The current execution state
     * @param value The value to check
     * @return true if value delegates behavior to this object, otherwise
     * false
     */
    virtual bool hasInstance(ExecState *exec, JSValue *value);

    void getPropertyNames(ExecState*, PropertyNameArray&, PropertyMap::PropertyMode mode = PropertyMap::ExcludeDontEnumProperties);
    virtual void getOwnPropertyNames(ExecState*, PropertyNameArray&, PropertyMap::PropertyMode mode);

    virtual JSValue *toPrimitive(ExecState *exec, JSType preferredType = UnspecifiedType) const;
    virtual bool getPrimitiveNumber(ExecState*, double& number, JSValue*& value);
    virtual bool toBoolean(ExecState *exec) const;
    virtual double toNumber(ExecState *exec) const;
    virtual UString toString(ExecState *exec) const;
    virtual JSObject *toObject(ExecState *exec) const;

    virtual bool getPropertyAttributes(const Identifier& propertyName, unsigned& attributes) const;

    // Returns whether the object should be treated as undefined when doing equality comparisons
    virtual bool masqueradeAsUndefined() const { return false; }

    // This get function only looks at the property map for Object.
    // It is virtual because for all custom-data classes we want to by-pass
    // the prototype and get it directly. For example called from
    // Object::defineOwnProperty to directly get GetterSetterImp and update it.
    // This is used e.g. by lookupOrCreateFunction (to cache a function, we don't want
    // to look up in the prototype, it might already exist there)
    virtual JSValue *getDirect(const Identifier& propertyName) const
        { return _prop.get(propertyName); }
    JSValue **getDirectLocation(const Identifier& propertyName)
        { return _prop.getLocation(propertyName); }

    // If this method returns non-0, there is already a property
    // with name propertyName that's not readonly and not a setter-getter
    // which can be updated via the returned pointer.
    JSValue **getDirectWriteLocation(const Identifier& propertyName)
        { return _prop.getWriteLocation(propertyName); }

    // This function is virtual to directly store, by-pass the prototype, values
    // for all custom-data classes like the Array. For example an Array with a prototype
    // to store values via getter/setter. It would be impossible to store a value
    // by-passing the getter/setter prototype.
    // This is for example called in Object::defineOwnProperty to directly store the values.
    // Same for removeDirect.
    virtual void putDirect(const Identifier &propertyName, JSValue *value, int attr = 0)
        { _prop.put(propertyName, value, attr); }
    virtual void putDirect(const Identifier &propertyName, int value, int attr = 0);
    virtual void removeDirect(const Identifier &propertyName);

    // convenience to add a function property under the function's own built-in name
    void putDirectFunction(InternalFunctionImp*, int attr = 0);

    void fillGetterPropertySlot(PropertySlot& slot, JSValue **location);
    void fillDirectLocationSlot(PropertySlot& slot, JSValue **location);

    void defineGetter(ExecState *exec, const Identifier& propertyName, JSObject *getterFunc);
    void defineSetter(ExecState *exec, const Identifier& propertyName, JSObject *setterFunc);

    virtual bool defineOwnProperty(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& desc, bool shouldThrow);

    void preventExtensions();
    bool isExtensible() { return _prop.isExtensible(); }

    /**
     * Remove all properties from this object.
     * This doesn't take DontDelete into account, and isn't in the ECMA spec.
     * It's simply a quick way to remove everything stored in the property map.
     */
    void clearProperties() { _prop.clear(); }

    void saveProperties(SavedProperties &p) const { _prop.save(p); }
    void restoreProperties(const SavedProperties &p) { _prop.restore(p); }

    virtual bool isActivation()   const { return false; }
    virtual bool isGlobalObject() const { return false; }

    // This is used to keep track of whether scope object have local
    // variables introduced by something other than 'var'
    bool isLocalInjected()  const { return _prop.m_objLocalInjected; }
    void setLocalInjected()       { _prop.m_objLocalInjected = true; }

  protected:
    PropertyMap _prop;
  private:


    const HashEntry* findPropertyHashEntry( const Identifier& propertyName ) const;
    JSValue *_proto;
#ifdef WIN32
    JSObject(const JSObject&);
    JSObject& operator=(const JSObject&);
#endif
  };

  /**
   * Types of Native Errors available. For custom errors, GeneralError
   * should be used.
   */
  enum ErrorType { GeneralError   = 0,
                   EvalError      = 1,
                   RangeError     = 2,
                   ReferenceError = 3,
                   SyntaxError    = 4,
                   TypeError      = 5,
                   URIError       = 6};

  /**
   * @short Factory methods for error objects.
   */
  class KJS_EXPORT Error {
  public:
    /**
     * Factory method for error objects.
     *
     * @param exec The current execution state
     * @param errtype Type of error.
     * @param message Optional error message.
     * @param lineNumber Optional line number.
     * @param sourceId Optional source id.
     * @param sourceURL Optional source URL.
     */
    static JSObject *create(ExecState *, ErrorType, const UString &message, int lineNumber, int sourceId, const UString &sourceURL);
    static JSObject *create(ExecState *, ErrorType, const char *message);

    /**
     * Array of error names corresponding to ErrorType
     */
    static const char * const * const errorNames;
  };

KJS_EXPORT JSObject *throwError(ExecState *, ErrorType, const UString &message, int lineNumber, int sourceId, const UString &sourceURL);
KJS_EXPORT JSObject *throwError(ExecState *, ErrorType, const UString &message);
KJS_EXPORT JSObject *throwError(ExecState *, ErrorType, const char *message);
KJS_EXPORT JSObject *throwError(ExecState *, ErrorType);

inline JSObject::JSObject(JSValue* proto)
    : _proto(proto)
{
    assert(proto);
}

inline JSObject::JSObject()
    :_proto(jsNull())
{}

inline JSValue *JSObject::prototype() const
{
    return _proto;
}

inline void JSObject::setPrototype(JSValue *proto)
{
    assert(proto);
    _proto = proto;
}

inline bool JSObject::inherits(const ClassInfo *info) const
{
    for (const ClassInfo *ci = classInfo(); ci; ci = ci->parentClass)
        if (ci == info)
            return true;
    return false;
}

inline void JSObject::fillDirectLocationSlot(PropertySlot& slot,
                                             JSValue **location)
{
    if (_prop.hasGetterSetterProperties() &&
        (*location)->type() == GetterSetterType)
        fillGetterPropertySlot(slot, location);
    else
        slot.setValueSlot(this, location);
}

// this method is here to be after the inline declaration of JSObject::inherits
inline bool JSCell::isObject(const ClassInfo *info) const
{
    return isObject() && static_cast<const JSObject *>(this)->inherits(info);
}

// this method is here to be after the inline declaration of JSCell::isObject
inline bool JSValue::isObject(const ClassInfo *c) const
{
    return !JSImmediate::isImmediate(this) && asCell()->isObject(c);
}

// It may seem crazy to inline a function this large but it makes a big difference
// since this is function very hot in variable lookup
inline bool JSObject::getPropertySlot(ExecState *exec, const Identifier& propertyName, PropertySlot& slot)
{
    JSObject *object = this;
    while (true) {
        if (object->getOwnPropertySlot(exec, propertyName, slot))
            return true;

        JSValue *proto = object->_proto;
        if (!proto->isObject())
            return false;

        object = static_cast<JSObject *>(proto);
    }
}

inline void JSObject::getPropertyNames(ExecState* exec, PropertyNameArray& propertyNames, PropertyMap::PropertyMode mode)
{
  for (JSObject* cur = this; cur; cur = cur->_proto->getObject())
    cur->getOwnPropertyNames(exec, propertyNames, mode);
}

inline JSValue* JSObject::toPrimitive(ExecState* exec, JSType preferredType) const
{
    return defaultValue(exec, preferredType);
}

inline JSValue* JSObject::call(ExecState *exec, JSObject *thisObj, const List &args)
{
	return callAsFunction(exec, thisObj, args);
}

} // namespace

#endif // KJS_OBJECT_H
