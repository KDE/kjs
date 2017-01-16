/*
 *  Copyright (C) 2003, 2004, 2005, 2006, 2007 Apple Inc. All rights reserved.
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

#include "list.h"

#include "internal.h"
#include <algorithm>

#define DUMP_STATISTICS 0

using std::min;

namespace KJS
{

// tunable parameters
const int poolSize = 512;

enum ListImpState { unusedInPool = 0, usedInPool, usedOnHeap, immortal };

struct ListImp : ListImpBase {
    ListImpState state;

    union {
        int capacity; // or 0 if data is inline
        ListImp *nextInFreeList;
    };

    LocalStorageEntry values[inlineListValuesSize];

#if DUMP_STATISTICS
    int sizeHighWaterMark;
#endif

    void markValues();
};

struct HeapListImp : ListImp {
    HeapListImp *nextInHeapList;
    HeapListImp *prevInHeapList;
};

static ListImp pool[poolSize];
static ListImp *poolFreeList;
static HeapListImp *heapList;
static int poolUsed;

#if DUMP_STATISTICS

static int numLists;
static int numListsHighWaterMark;

static int listSizeHighWaterMark;

static int numListsDestroyed;
static int numListsBiggerThan[17];

struct ListStatisticsExitLogger {
    ~ListStatisticsExitLogger();
};

static ListStatisticsExitLogger logger;

ListStatisticsExitLogger::~ListStatisticsExitLogger()
{
    printf("\nKJS::List statistics:\n\n");
    printf("%d lists were allocated\n", numLists);
    printf("%d lists was the high water mark\n", numListsHighWaterMark);
    printf("largest list had %d elements\n", listSizeHighWaterMark);
    if (numListsDestroyed) {
        putc('\n', stdout);
        for (int i = 0; i < 17; i++) {
            printf("%.1f%% of the lists (%d) had more than %d element%s\n",
                   100.0 * numListsBiggerThan[i] / numListsDestroyed,
                   numListsBiggerThan[i],
                   i, i == 1 ? "" : "s");
        }
        putc('\n', stdout);
    }
}

#endif

inline void ListImp::markValues()
{
    for (int i = 0; i != size; ++i) {
        if (!data[i].val.valueVal->marked()) {
            data[i].val.valueVal->mark();
        }
    }
}

void List::markProtectedLists()
{
    int seen = 0;
    int used = poolUsed;

    for (int i = 0; i < poolSize && seen < used; i++) {
        if (pool[i].state == usedInPool) {
            seen++;
            pool[i].markValues();
        }
    }

    for (HeapListImp *l = heapList; l; l = l->nextInHeapList) {
        l->markValues();
    }
}

static inline ListImp *allocateListImp()
{
    // Find a free one in the pool.
    if (poolUsed < poolSize) {
        ListImp *imp = poolFreeList ? poolFreeList : &pool[0];
        poolFreeList = imp->nextInFreeList ? imp->nextInFreeList : imp + 1;
        imp->state = usedInPool;
        poolUsed++;
        return imp;
    }

    HeapListImp *imp = new HeapListImp;
    imp->state = usedOnHeap;
    // link into heap list
    if (heapList) {
        heapList->prevInHeapList = imp;
    }
    imp->nextInHeapList = heapList;
    imp->prevInHeapList = nullptr;
    heapList = imp;

    return imp;
}

List::List() : _impBase(allocateListImp())
{
    ListImp *imp = static_cast<ListImp *>(_impBase);
    imp->size = 0;
    imp->refCount = 1;
    imp->capacity = 0;
    imp->data     = imp->values;

#if DUMP_STATISTICS
    if (++numLists > numListsHighWaterMark) {
        numListsHighWaterMark = numLists;
    }
    imp->sizeHighWaterMark = 0;
#endif
}

void List::release()
{
    ListImp *imp = static_cast<ListImp *>(_impBase);

#if DUMP_STATISTICS
    if (imp->size > imp->sizeHighWaterMark) {
        imp->sizeHighWaterMark = imp->size;
    }

    --numLists;
    ++numListsDestroyed;
    for (int i = 0; i < 17; i++)
        if (imp->sizeHighWaterMark > i) {
            ++numListsBiggerThan[i];
        }
#endif

    if (imp->capacity) {
        delete [] imp->data;
    }
    imp->data = nullptr;

    if (imp->state == usedInPool) {
        imp->state = unusedInPool;
        imp->nextInFreeList = poolFreeList;
        poolFreeList = imp;
        poolUsed--;
    } else {
        assert(imp->state == usedOnHeap);
        HeapListImp *list = static_cast<HeapListImp *>(imp);

        // unlink from heap list
        if (!list->prevInHeapList) {
            heapList = list->nextInHeapList;
            if (heapList) {
                heapList->prevInHeapList = nullptr;
            }
        } else {
            list->prevInHeapList->nextInHeapList = list->nextInHeapList;
            if (list->nextInHeapList) {
                list->nextInHeapList->prevInHeapList = list->prevInHeapList;
            }
        }

        delete list;
    }
}

void List::appendSlowCase(JSValue *v)
{
    ListImp *imp = static_cast<ListImp *>(_impBase);

    int i = imp->size++; // insert index/old size

#if DUMP_STATISTICS
    if (imp->size > listSizeHighWaterMark) {
        listSizeHighWaterMark = imp->size;
    }
#endif

    // If we got here, we need to use an out-of-line buffer.

    if (i >= imp->capacity) {
        int newCapacity = i * 2;

        LocalStorageEntry *newBuffer = new LocalStorageEntry[newCapacity];

        // Copy everything over
        for (int c = 0; c < i; ++c) {
            newBuffer[c] = imp->data[c];
        }

        if (imp->capacity) { // had an old out-of-line buffer
            delete[] imp->data;
        }

        imp->data     = newBuffer;
        imp->capacity = newCapacity;
    }

    imp->data[i].val.valueVal = v;
}

List List::copy() const
{
    List copy;
    copy.copyFrom(*this);
    return copy;
}

void List::copyFrom(const List &other)
{
    // Assumption: we're empty (e.g. called from copy)
    ListImpBase *otherImp = other._impBase;
    ListImp *ourImp       = static_cast<ListImp *>(_impBase);

    assert(ourImp->size == 0 && ourImp->capacity == 0);

    int size = otherImp->size;
    ourImp->size     = size;

    if (size > inlineListValuesSize) {
        // need an out-of-line buffer
        ourImp->capacity = size;
        ourImp->data     = new LocalStorageEntry[size];
    } else {
        ourImp->capacity = 0;
    }

    for (int c = 0; c < size; ++c) {
        ourImp->data[c] = otherImp->data[c];
    }
}

List List::copyTail() const
{
    List copy;

    ListImpBase *inImp  = _impBase;
    ListImp     *outImp = static_cast<ListImp *>(copy._impBase);

    int size      = inImp->size - 1;

    if (size < 0) {
        size = 0;    // copyTail on empty list.
    }

    outImp->size  = size;

    if (size > inlineListValuesSize) {
        // need an out-of-line buffer
        outImp->capacity = size;
        outImp->data     = new LocalStorageEntry[size];
    } else {
        outImp->capacity = 0;
    }

    for (int c = 0; c < size; ++c) {
        outImp->data[c] = inImp->data[c + 1];
    }

    return copy;
}

const List &List::empty()
{
    static List emptyList;
    return emptyList;
}

List &List::operator=(const List &b)
{
    ListImpBase *bImpBase = b._impBase;
    ++bImpBase->refCount;
    deref();
    _impBase = bImpBase;
    return *this;
}

} // namespace KJS

