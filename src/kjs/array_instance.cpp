// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2003, 2007, 2008 Apple Inc. All rights reserved.
 *  Copyright (C) 2003 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2006 Alexey Proskuryakov (ap@nypop.com)
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

#include "array_instance.h"

#include "PropertyNameArray.h"
#include "JSVariableObject.h"
#include <wtf/Assertions.h>
#include <wtf/HashMap.h>

#include <algorithm>
#include <stdio.h>

using std::min;
using std::max;


namespace KJS {

struct ArrayEntity {
    ArrayEntity()
        : value(0),
          attributes(0)
    {
    }

    ArrayEntity(JSValue* jsVal, uint32_t attr)
        : value(jsVal),
          attributes(attr)
    {
    }

    JSValue* value;
    uint32_t attributes;
};

typedef HashMap<unsigned, ArrayEntity> SparseArrayValueMap;

struct ArrayStorage {
    unsigned m_numValuesInVector;
    SparseArrayValueMap* m_sparseValueMap;
    ArrayEntity m_vector[1];
};

// (2^32)-1
static const unsigned maxArrayLength = 0xFFFFFFFFU;
// 0xFFFFFFFF is a bit weird -- is not an array index even though it's an integer
static const unsigned maxArrayIndex = 0xFFFFFFFEU;

// Our policy for when to use a vector and when to use a sparse map.
// For all array indices under sparseArrayCutoff, we always use a vector.
// When indices greater than sparseArrayCutoff are involved, we use a vector
// as long as it is 1/8 full. If more sparse than that, we use a map.
static const unsigned sparseArrayCutoff = 10000;
static const unsigned minDensityMultiplier = 8;

static const unsigned mergeSortCutoff = 10000;

const ClassInfo ArrayInstance::info = {"Array", 0, 0, 0};

static inline size_t storageSize(unsigned vectorLength)
{
    return sizeof(ArrayStorage) - sizeof(ArrayEntity) + vectorLength * sizeof(ArrayEntity);
}

static inline unsigned increasedVectorLength(unsigned newLength)
{
    return (newLength * 3 + 1) / 2;
}

static inline bool isDenseEnoughForVector(unsigned length, unsigned numValues)
{
    return length / minDensityMultiplier <= numValues;
}

ArrayInstance::ArrayInstance(JSObject* prototype, unsigned initialLength)
    : JSObject(prototype)
{
    unsigned initialCapacity = min(initialLength, sparseArrayCutoff);

    m_length = initialLength;
    m_vectorLength = initialCapacity;
    m_storage = static_cast<ArrayStorage*>(fastCalloc(storageSize(initialCapacity), 1));
    m_lengthAttributes = DontDelete | DontEnum;

    Collector::reportExtraMemoryCost(initialCapacity * sizeof(ArrayEntity));
}

ArrayInstance::ArrayInstance(JSObject* prototype, const List& list)
    : JSObject(prototype)
{
    unsigned length = list.size();

    m_length = length;
    m_vectorLength = length;
    m_lengthAttributes = DontDelete | DontEnum;

    ArrayStorage* storage = static_cast<ArrayStorage*>(fastMalloc(storageSize(length)));

    storage->m_numValuesInVector = length;
    storage->m_sparseValueMap = 0;

    ListIterator it = list.begin();
    for (unsigned i = 0; i < length; ++i) {
        storage->m_vector[i].value = it++;
        storage->m_vector[i].attributes = 0;
    }

    m_storage = storage;

    // When the array is created non-empty, its cells are filled, so it's really no worse than
    // a property map. Therefore don't report extra memory cost.
}

ArrayInstance::~ArrayInstance()
{
    delete m_storage->m_sparseValueMap;
    fastFree(m_storage);
}

JSValue* ArrayInstance::getItem(unsigned i) const
{
    ASSERT(i <= maxArrayIndex);

    ArrayEntity* ent = getArrayEntity(i);
    if (ent)
        return ent->value;
    return jsUndefined();
}

JSValue* ArrayInstance::lengthGetter(ExecState*, JSObject*, const Identifier&, const PropertySlot& slot)
{
    return jsNumber(static_cast<ArrayInstance*>(slot.slotBase())->m_length);
}

ALWAYS_INLINE bool ArrayInstance::inlineGetOwnPropertySlot(ExecState* exec, unsigned i, PropertySlot& slot)
{
    if (i >= m_length) {
        if (i > maxArrayIndex)
            return getOwnPropertySlot(exec, Identifier::from(i), slot);
        return false;
    }

    ArrayEntity* ent = getArrayEntity(i);
    if (ent) {
        if (ent->attributes & GetterSetter) {
            GetterSetterImp *gs = static_cast<GetterSetterImp *>(ent->value);
            JSObject *getterFunc = gs->getGetter();
            if (getterFunc)
                slot.setGetterSlot(this, getterFunc);
            else
                slot.setUndefined(this);
            return true;
        }
        slot.setValueSlot(this, &ent->value);
        return true;
    }

    return false;
}

ArrayEntity* ArrayInstance::getArrayEntity(unsigned int i) const
{
    if (i >= m_length)
        return 0;

    ArrayStorage* storage = m_storage;
    if (i < m_vectorLength) {
        if (storage->m_vector[i].value)
            return &storage->m_vector[i];
    }

    SparseArrayValueMap* map = storage->m_sparseValueMap;
    if (map && i > 0 && i <= maxArrayIndex) {
        SparseArrayValueMap::iterator it = map->find(i);
        if (it != map->end()) {
            return &it->second;
        }
    }

    return 0;
}

bool ArrayInstance::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    if (propertyName == exec->propertyNames().length) {
        slot.setCustom(this, lengthGetter);
        return true;
    }

    bool isArrayIndex;
    unsigned i = propertyName.toArrayIndex(&isArrayIndex);
    if (isArrayIndex)
        return inlineGetOwnPropertySlot(exec, i, slot);

    return JSObject::getOwnPropertySlot(exec, propertyName, slot);
}

bool ArrayInstance::getOwnPropertySlot(ExecState* exec, unsigned i, PropertySlot& slot)
{
    return inlineGetOwnPropertySlot(exec, i, slot);
}

// ECMA 15.4.5.1
void ArrayInstance::put(ExecState* exec, const Identifier& propertyName, JSValue* value, int attributes)
{
    bool isArrayIndex;
    unsigned i = propertyName.toArrayIndex(&isArrayIndex);
    if (isArrayIndex) {
        put(exec, i, value, attributes);
        return;
    }

    if (propertyName == exec->propertyNames().length) {
        if (m_lengthAttributes & ReadOnly)
            return;
        unsigned newLength = value->toUInt32(exec);
        if (value->toNumber(exec) != static_cast<double>(newLength)) {
            throwError(exec, RangeError, "Invalid array length.");
            return;
        }
        m_lengthAttributes = attributes;
        setLength(newLength);
        return;
    }

    JSObject::put(exec, propertyName, value, attributes);
}

void ArrayInstance::put(ExecState* exec, unsigned i, JSValue* value, int attributes)
{
    ArrayEntity *ent = getArrayEntity(i);
    if (ent) {
        if (ent->attributes & ReadOnly)
            return;
        attributes |= ent->attributes;

        JSValue* gs = ent->value;
        if (gs && !gs->isUndefined()) {
            if (ent->attributes & GetterSetter) {
                JSObject *setterFunc = static_cast<GetterSetterImp *>(gs)->getSetter();

                if (!setterFunc) {
                    if (false) //if strict is true
                        throwError(exec, TypeError, "setting a property that has only a getter");
                    return;
                }

                List args;
                args.append(value);

                setterFunc->call(exec, this, args);
                return;
            }
        }
    }

    KJS::ArrayInstance::putDirect(i, value, attributes);
}

bool ArrayInstance::deleteProperty(ExecState* exec, const Identifier& propertyName)
{
    bool isArrayIndex;
    unsigned i = propertyName.toArrayIndex(&isArrayIndex);
    if (isArrayIndex)
        return deleteProperty(exec, i);

    if (propertyName == exec->propertyNames().length)
        return false;

    return JSObject::deleteProperty(exec, propertyName);
}

bool ArrayInstance::deleteProperty(ExecState* exec, unsigned i)
{
    ArrayStorage* storage = m_storage;

    if (i < m_vectorLength) {
        ArrayEntity* ent = &storage->m_vector[i];
        if (ent->value) {
            if (ent->attributes & DontDelete)
                return false;

            JSValue*& valueSlot = ent->value;
            bool hadValue = valueSlot;
            valueSlot = 0;
            storage->m_numValuesInVector -= hadValue;
            ent->value = 0;
            return hadValue;
        }
    }

    if (SparseArrayValueMap* map = storage->m_sparseValueMap) {
        SparseArrayValueMap::iterator it = map->find(i);
        if (it != map->end()) {
            if ((*it).second.attributes & DontDelete)
                return false;
            map->remove(it->first);
            return true;
        }
    }

    if (i > maxArrayIndex)
        return JSObject::deleteProperty(exec, Identifier::from(i));

    return true;
}

void ArrayInstance::getOwnPropertyNames(ExecState* exec, PropertyNameArray& propertyNames, PropertyMap::PropertyMode mode)
{
    // FIXME: Filling PropertyNameArray with an identifier for every integer
    // is incredibly inefficient for large arrays. We need a different approach.

    ArrayStorage* storage = m_storage;

    unsigned usedVectorLength = min(m_length, m_vectorLength);
    for (unsigned i = 0; i < usedVectorLength; ++i) {
        if (storage->m_vector[i].value &&
            (!(storage->m_vector[i].attributes & DontEnum) ||
             mode == PropertyMap::IncludeDontEnumProperties))
            propertyNames.add(Identifier::from(i));
    }

    if (SparseArrayValueMap* map = storage->m_sparseValueMap) {
        SparseArrayValueMap::iterator end = map->end();
        for (SparseArrayValueMap::iterator it = map->begin(); it != end; ++it)
            if (!((*it).second.attributes & DontEnum) ||
                mode == PropertyMap::IncludeDontEnumProperties)
                propertyNames.add(Identifier::from(it->first));
    }

    if (mode == PropertyMap::IncludeDontEnumProperties)
        propertyNames.add(exec->propertyNames().length);

    JSObject::getOwnPropertyNames(exec, propertyNames, mode);
}

bool ArrayInstance::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    if (propertyName == exec->propertyNames().length) {
        descriptor.setPropertyDescriptorValues(exec, jsNumber(m_length), m_lengthAttributes);
        return true;
    }

    bool isArrayIndex;
    unsigned i = propertyName.toArrayIndex(&isArrayIndex);

    if (isArrayIndex) {
        ArrayEntity* ent = getArrayEntity(i);
        if (ent) {
            descriptor.setPropertyDescriptorValues(exec, ent->value, ent->attributes);
            return true;
        }
    }
    return JSObject::getOwnPropertyDescriptor(exec, propertyName, descriptor);
}


//ECMAScript Edition 5.1r6 - 15.4.5.1
bool ArrayInstance::defineOwnProperty(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& desc, bool shouldThrow)
{
    PropertyDescriptor oldLenDesc;
    getOwnPropertyDescriptor(exec, exec->propertyNames().length, oldLenDesc);
    unsigned int oldLen = oldLenDesc.value()->toUInt32(exec);

    //4a
    bool isArrayIndex;
    unsigned index = propertyName.toArrayIndex(&isArrayIndex);

    //Step 3
    if (propertyName == exec->propertyNames().length) {
        //a
        if (!desc.value())
            return JSObject::defineOwnProperty(exec, propertyName, desc, shouldThrow);

        //b
        PropertyDescriptor newLenDesc(desc);
        //c
        unsigned int newLen = desc.value()->toUInt32(exec);
        //d
        if (newLen != desc.value()->toNumber(exec) || desc.value()->toNumber(exec) > maxArrayLength) {
            throwError(exec, RangeError, "Index out of range");
            return false;
        }
        //e
        newLenDesc.setValue(jsNumber(newLen));
        //f
        if (newLen >= oldLen)
            return JSObject::defineOwnProperty(exec, propertyName, newLenDesc, shouldThrow);
        //g
        if (!oldLenDesc.writable()) {
            if (shouldThrow)
                throwError(exec, TypeError, "length is not writable");
            return false;
        }
        //h
        bool newWriteable;
        if (!newLenDesc.writableSet() || newLenDesc.writable()) {
            newWriteable = true;
        } else { //i
            if (anyItemHasAttribute(DontDelete))
                newLenDesc.setWritable(false);
            else
                newLenDesc.setWritable(true);
            newWriteable = false;
        }
        //j
        bool succeeded = JSObject::defineOwnProperty(exec, propertyName, newLenDesc, shouldThrow);
        //k
        if (!succeeded) return false;
        //l
        while (newLen < oldLen) {
            --oldLen;
            bool deleteSucceeded = deleteProperty(exec, oldLen); // 3. argument should be false
            if (!deleteSucceeded) {
                newLenDesc.setValue(jsNumber(oldLen+1));
                if (!newWriteable)
                    newLenDesc.setWritable(false);
                JSObject::defineOwnProperty(exec, propertyName, newLenDesc, false);
                if (shouldThrow)
                    throwError(exec, TypeError);
                return false;
            }
        }
        //m
        if (!newWriteable) {
            PropertyDescriptor writableDesc;
            writableDesc.setWritable(false);
            return JSObject::defineOwnProperty(exec, propertyName, writableDesc, false);
        }
        return true;
    } else if (isArrayIndex) {//Step 4
        //b
        if (index >= oldLen && !oldLenDesc.writable()) {
            if (shouldThrow)
                throwError(exec, TypeError);
            return false;
        }
        //c
        bool succeeded = JSObject::defineOwnProperty(exec, propertyName, desc, false);
        //d
        if (!succeeded) {
            if (shouldThrow)
                throwError(exec, TypeError);
            return false;
        }
        //e
        if (index >= oldLen) {
            oldLenDesc.setValue(jsNumber(index+1));
            JSObject::defineOwnProperty(exec, exec->propertyNames().length, oldLenDesc, false);
        }
        return true;
    }

    return JSObject::defineOwnProperty(exec, propertyName, desc, shouldThrow);
}

bool ArrayInstance::getPropertyAttributes(const Identifier& propertyName, unsigned int& attributes) const
{
    bool isArrayIndex;
    unsigned i = propertyName.toArrayIndex(&isArrayIndex);

    if (isArrayIndex) {
        ArrayEntity* ent = getArrayEntity(i);
        if (ent) {
            attributes = ent->attributes;
            return true;
        }
    }

    return KJS::JSObject::getPropertyAttributes(propertyName, attributes);
}

JSValue* ArrayInstance::getDirect(const Identifier& propertyName) const
{
    bool isArrayIndex;
    unsigned i = propertyName.toArrayIndex(&isArrayIndex);

    if (isArrayIndex) {
        ArrayEntity* ent = getArrayEntity(i);
        if (ent) {
            if (ent->value)
                return ent->value;
        }
    }

    return KJS::JSObject::getDirect(propertyName);
}

void ArrayInstance::putDirect(unsigned i, JSValue* value, int attributes)
{
    unsigned length = m_length;

    if (i >= length) {
        if (i > maxArrayIndex) {
            KJS::JSObject::putDirect(Identifier::from(i), value, attributes);
            return;
        }
        length = i + 1;
        m_length = length;
    }

    ArrayStorage* storage = m_storage;

    if (i < m_vectorLength) {
        ArrayEntity* ent = &storage->m_vector[i];
        if (!ent->value && !isExtensible())
            return;
        JSValue*& valueSlot = ent->value;
        storage->m_numValuesInVector += !valueSlot;
        valueSlot = value;
        ent->attributes = attributes;
        return;
    }

    if (!isExtensible())
        return;

    SparseArrayValueMap* map = storage->m_sparseValueMap;
    if (i >= sparseArrayCutoff) {
        // If the index is high, go to the map unless we're pretty dense.
        if (!map) {
            map = new SparseArrayValueMap;
            storage->m_sparseValueMap = map;

            // If we create a sparse map, we need to ensure that there is at least one spot
            // in the vector map, however, since the sparse map can't put/get key 0.
            // It's safe to do it here, since put(0) will always put it in the vector part,
            // but we have to do it before a get(0) or it will crash
            if (!m_vectorLength)
                increaseVectorLength(1);
        }

        map->set(i, ArrayEntity(value, attributes));
        return;
    }

    // note: an invariant here is that indeces < sparseArrayCutoff
    // are always inside the vector portion.

    // lowish indeces or high density -> we have decided that we'll put the new item into the vector.
    // Fast case is when there is no sparse map, so we can increase the vector size without moving values from the sparse map.
    if (!map || map->isEmpty()) {
        increaseVectorLength(i + 1);
        storage = m_storage;
        ++storage->m_numValuesInVector;
        storage->m_vector[i].value = value;
        storage->m_vector[i].attributes = attributes;
        return;
    }

    // Decide just how large we want to make the vector to be.
    unsigned newNumValuesInVector = storage->m_numValuesInVector + 1;
    unsigned newVectorLength = increasedVectorLength(i + 1);

    // First, count how much stuff we are guaranteed to move over, now
    // that we've decided to at least include i in the vector.
    for (unsigned j = max(m_vectorLength, sparseArrayCutoff); j < newVectorLength; ++j)
        newNumValuesInVector += map->contains(j);
    if (i >= sparseArrayCutoff)
        newNumValuesInVector -= map->contains(i);

    // Continue increasing the vector portion as long as the things in the map are dense enough
    if (isDenseEnoughForVector(newVectorLength, newNumValuesInVector)) {
        unsigned proposedNewNumValuesInVector = newNumValuesInVector;
        while (true) {
            unsigned proposedNewVectorLength = increasedVectorLength(newVectorLength + 1);
            for (unsigned j = max(newVectorLength, sparseArrayCutoff); j < proposedNewVectorLength; ++j)
                proposedNewNumValuesInVector += map->contains(j);
            if (!isDenseEnoughForVector(proposedNewVectorLength, proposedNewNumValuesInVector))
                break;
            newVectorLength = proposedNewVectorLength;
            newNumValuesInVector = proposedNewNumValuesInVector;
        }
    }

    storage = static_cast<ArrayStorage*>(fastRealloc(storage, storageSize(newVectorLength)));

    unsigned vectorLength = m_vectorLength;

    // Special case: if we just added a single value, we don't have to scan the map
    // to see what to remove from it
    if (newNumValuesInVector == storage->m_numValuesInVector + 1) {
        for (unsigned j = vectorLength; j < newVectorLength; ++j)
            storage->m_vector[j].value = 0;
        if (i > sparseArrayCutoff)
            map->remove(i);
    } else {
        // Move over things from the map to the new array portion
        for (unsigned j = vectorLength; j < max(vectorLength, sparseArrayCutoff); ++j)
            storage->m_vector[j].value = 0;
        for (unsigned j = max(vectorLength, sparseArrayCutoff); j < newVectorLength; ++j)
            storage->m_vector[j] = map->take(j);
    }

    storage->m_vector[i].value = value;
    storage->m_vector[i].attributes = attributes;

    m_vectorLength = newVectorLength;
    storage->m_numValuesInVector = newNumValuesInVector;

    m_storage = storage;
}

void ArrayInstance::putDirect(const Identifier& propertyName, JSValue* value, int attr)
{
    bool isArrayIndex;
    unsigned i = propertyName.toArrayIndex(&isArrayIndex);

    if (isArrayIndex) {
        KJS::ArrayInstance::putDirect(i, value, attr);
        return;
    }

    KJS::JSObject::putDirect(propertyName, value, attr);
}

void ArrayInstance::putDirect(const Identifier& propertyName, int value, int attr)
{
    bool isArrayIndex;
    unsigned i = propertyName.toArrayIndex(&isArrayIndex);

    if (isArrayIndex) {
        KJS::ArrayInstance::putDirect(i, jsNumber(value), attr);
        return;
    }

    KJS::JSObject::putDirect(propertyName, jsNumber(value), attr);
}

void ArrayInstance::removeDirect(const Identifier& propertyName)
{
    bool isArrayIndex;
    unsigned i = propertyName.toArrayIndex(&isArrayIndex);

    if (isArrayIndex) {
        ArrayStorage* storage = m_storage;

        if (i < m_vectorLength) {
            ArrayEntity* ent = &storage->m_vector[i];
            if (ent->value) {
                JSValue*& valueSlot = ent->value;
                bool hadValue = valueSlot;
                valueSlot = 0;
                storage->m_numValuesInVector -= hadValue;
                ent->value = 0;
                return;
            }
        }

        if (SparseArrayValueMap* map = storage->m_sparseValueMap) {
            SparseArrayValueMap::iterator it = map->find(i);
            if (it != map->end()) {
                map->remove(it->first);
                return;
            }
        }
    }

    JSObject::removeDirect(Identifier::from(i));
}

void ArrayInstance::increaseVectorLength(unsigned newLength)
{
    // This function leaves the array in an internally inconsistent state, because it does not move any values from sparse value map
    // to the vector. Callers have to account for that, because they can do it more efficiently.

    ArrayStorage* storage = m_storage;

    unsigned vectorLength = m_vectorLength;
    ASSERT(newLength > vectorLength);
    unsigned newVectorLength = increasedVectorLength(newLength);

    storage = static_cast<ArrayStorage*>(fastRealloc(storage, storageSize(newVectorLength)));
    m_vectorLength = newVectorLength;

    for (unsigned i = vectorLength; i < newVectorLength; ++i)
        storage->m_vector[i].value = 0;

    m_storage = storage;
}

void ArrayInstance::setLength(unsigned newLength)
{
    ArrayStorage* storage = m_storage;

    unsigned length = m_length;
    unsigned newLenSet = newLength;

    if (newLength < length) {
        unsigned usedVectorLength = min(length, m_vectorLength);
        if (usedVectorLength > 0) {
            for (unsigned i = usedVectorLength-1; i >= newLength && i > 0; --i) {
                ArrayEntity* ent = &storage->m_vector[i];
                if (ent->value) {
                    if (ent->attributes & DontDelete) {
                        newLenSet = i+1;
                        break;
                    }
                    JSValue*& valueSlot = ent->value;
                    bool hadValue = valueSlot;
                    valueSlot = 0;
                    ent->value = 0;
                    storage->m_numValuesInVector -= hadValue;
                }
            }
        }

        if (SparseArrayValueMap* map = storage->m_sparseValueMap) {
            SparseArrayValueMap copy = *map;
            SparseArrayValueMap::iterator end = copy.end();
            for (SparseArrayValueMap::iterator it = copy.begin(); it != end; ++it) {
                if (it->first >= newLength) {
                    if (it->second.attributes & DontDelete) {
                        newLenSet = it->first + 1;
                    }
                }
            }

            for (SparseArrayValueMap::iterator it = copy.begin(); it != end; ++it) {
                if (it->first >= newLenSet) {
                    map->remove(it->first);
                }
            }

            if (map->isEmpty()) {
                delete map;
                storage->m_sparseValueMap = 0;
            }
        }
    }

    m_length = newLenSet;
}

void ArrayInstance::mark()
{
    JSObject::mark();

    ArrayStorage* storage = m_storage;

    unsigned usedVectorLength = min(m_length, m_vectorLength);
    for (unsigned i = 0; i < usedVectorLength; ++i) {
        ArrayEntity* ent = &storage->m_vector[i];
        if (ent->value && !ent->value->marked())
            ent->value->mark();
    }

    if (SparseArrayValueMap* map = storage->m_sparseValueMap) {
        SparseArrayValueMap::iterator end = map->end();
        for (SparseArrayValueMap::iterator it = map->begin(); it != end; ++it) {
            ArrayEntity* ent = &it->second;
            if (!ent->value->marked())
                ent->value->mark();
        }
    }
}

static ExecState* execForCompareByStringForQSort;

static int compareByStringForQSort(const void* a, const void* b)
{
    ExecState* exec = execForCompareByStringForQSort;

    const ArrayEntity* va = static_cast<const ArrayEntity*>(a);
    const ArrayEntity* vb = static_cast<const ArrayEntity*>(b);

    ASSERT(va->value && !va->value->isUndefined());
    ASSERT(vb->value && !vb->value->isUndefined());

    return compare(va->value->toString(exec), vb->value->toString(exec));
}

void ArrayInstance::sort(ExecState* exec)
{
    unsigned lengthNotIncludingUndefined = compactForSorting();

    ExecState* oldExec = execForCompareByStringForQSort;
    execForCompareByStringForQSort = exec;

#if HAVE(MERGESORT)
    // Because mergesort usually does fewer compares, it is faster than qsort here.
    // However, because it requires extra copies of the storage buffer, don't use it for very
    // large arrays.

    // FIXME: Since we sort by string value, a fast algorithm might be to convert all the
    // values to string once up front, and then use a radix sort. That would be O(N) rather
    // than O(N log N).

    if (lengthNotIncludingUndefined < mergeSortCutoff) {
        // During the sort, we could do a garbage collect, and it's important to still
        // have references to every object in the array for ArrayInstance::mark.
        // The mergesort algorithm does not guarantee this, so we sort a copy rather
        // than the original.
        size_t size = storageSize(m_vectorLength);
        ArrayStorage* copy = static_cast<ArrayStorage*>(fastMalloc(size));
        memcpy(copy, m_storage, size);
        mergesort(copy->m_vector, lengthNotIncludingUndefined, sizeof(ArrayEntity), compareByStringForQSort);
        fastFree(m_storage);
        m_storage = copy;
        execForCompareByStringForQSort = oldExec;
        return;
    }
#endif

    qsort(m_storage->m_vector, lengthNotIncludingUndefined, sizeof(ArrayEntity), compareByStringForQSort);
    execForCompareByStringForQSort = oldExec;
}

struct CompareWithCompareFunctionArguments {
    CompareWithCompareFunctionArguments(ExecState *e, JSObject *cf)
        : exec(e)
        , compareFunction(cf)
        , globalObject(e->dynamicInterpreter()->globalObject())
    {
    }

    ExecState *exec;
    JSObject *compareFunction;
    List arguments;
    JSObject *globalObject;
};

static CompareWithCompareFunctionArguments* compareWithCompareFunctionArguments;

static int compareWithCompareFunctionForQSort(const void* a, const void* b)
{
    CompareWithCompareFunctionArguments *args = compareWithCompareFunctionArguments;

    const ArrayEntity* va = static_cast<const ArrayEntity*>(a);
    const ArrayEntity* vb = static_cast<const ArrayEntity*>(b);

    ASSERT(va->value && !va->value->isUndefined());
    ASSERT(vb->value && !vb->value->isUndefined());

    args->arguments.clear();
    args->arguments.append(va->value);
    args->arguments.append(vb->value);
    double compareResult = args->compareFunction->call
        (args->exec, args->globalObject, args->arguments)->toNumber(args->exec);
    return compareResult < 0 ? -1 : compareResult > 0 ? 1 : 0;
}

void ArrayInstance::sort(ExecState* exec, JSObject* compareFunction)
{
    size_t lengthNotIncludingUndefined = compactForSorting();

    CompareWithCompareFunctionArguments* oldArgs = compareWithCompareFunctionArguments;
    CompareWithCompareFunctionArguments args(exec, compareFunction);
    compareWithCompareFunctionArguments = &args;

#if HAVE(MERGESORT)
    // Because mergesort usually does fewer compares, it is faster than qsort here.
    // However, because it requires extra copies of the storage buffer, don't use it for very
    // large arrays.

    // FIXME: A tree sort using a perfectly balanced tree (e.g. an AVL tree) could do an even
    // better job of minimizing compares.

    if (lengthNotIncludingUndefined < mergeSortCutoff) {
        // During the sort, we could do a garbage collect, and it's important to still
        // have references to every object in the array for ArrayInstance::mark.
        // The mergesort algorithm does not guarantee this, so we sort a copy rather
        // than the original.
        size_t size = storageSize(m_vectorLength);
        ArrayStorage* copy = static_cast<ArrayStorage*>(fastMalloc(size));
        memcpy(copy, m_storage, size);
        mergesort(copy->m_vector, lengthNotIncludingUndefined, sizeof(ArrayEntity), compareWithCompareFunctionForQSort);
        fastFree(m_storage);
        m_storage = copy;
        compareWithCompareFunctionArguments = oldArgs;
        return;
    }
#endif

    qsort(m_storage->m_vector, lengthNotIncludingUndefined, sizeof(ArrayEntity), compareWithCompareFunctionForQSort);
    compareWithCompareFunctionArguments = oldArgs;
}


unsigned ArrayInstance::compactForSorting()
{
    ArrayStorage* storage = m_storage;

    unsigned usedVectorLength = min(m_length, m_vectorLength);

    unsigned numDefined = 0;
    unsigned numUndefined = 0;

    // This compacts normal values (e.g. not undefined) in a contiguous run
    // at the beginning of the array, and then puts any set undefined values
    // at the end

    // count the first contiguous run of defined values in the vector store
    for (; numDefined < usedVectorLength; ++numDefined) {
        ArrayEntity* v = &storage->m_vector[numDefined];
        if (!v->value || v->value->isUndefined())
            break;
    }

    // compact the rest, counting along the way
    for (unsigned i = numDefined; i < usedVectorLength; ++i) {
        ArrayEntity v = storage->m_vector[i];
        if (!v.value || v.value->isUndefined())
            ++numUndefined;
        else
            storage->m_vector[numDefined++] = storage->m_vector[i];
    }

    unsigned newUsedVectorLength = numDefined + numUndefined;

    if (SparseArrayValueMap* map = storage->m_sparseValueMap) {
        newUsedVectorLength += map->size();
        if (newUsedVectorLength > m_vectorLength) {
            increaseVectorLength(newUsedVectorLength);
            storage = m_storage;
        }

        SparseArrayValueMap::iterator end = map->end();
        for (SparseArrayValueMap::iterator it = map->begin(); it != end; ++it)
            storage->m_vector[numDefined++] = it->second;

        delete map;
        storage->m_sparseValueMap = 0;
    }

    for (unsigned i = numDefined; i < newUsedVectorLength; ++i)
        storage->m_vector[i].value = 0;
    for (unsigned i = newUsedVectorLength; i < usedVectorLength; ++i)
        storage->m_vector[i].value = 0;

    return numDefined;
}

bool KJS::ArrayInstance::anyItemHasAttribute(unsigned int attributes) const
{
    ArrayStorage* storage = m_storage;

    unsigned usedVectorLength = min(m_length, m_vectorLength);
    for (unsigned i = 0; i < usedVectorLength; ++i) {
        if (storage->m_vector[i].value && storage->m_vector[i].attributes & attributes)
            return true;
    }

    if (SparseArrayValueMap* map = storage->m_sparseValueMap) {
        SparseArrayValueMap::iterator end = map->end();
        for (SparseArrayValueMap::iterator it = map->begin(); it != end; ++it)
            if ((*it).second.attributes & attributes)
                return true;
    }

    return false;
}

}
// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on; hl c++;
