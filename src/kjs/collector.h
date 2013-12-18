/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2001 Peter Kelly (pmk@post.com)
 *  Copyright (C) 2003, 2004, 2005, 2006, 2007 Apple Inc. All rights reserved.
 *  Copyright (C) 2007 Maksim Orlovich (maksim@kde.org)
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

#ifndef KJSCOLLECTOR_H_
#define KJSCOLLECTOR_H_

#include <wtf/HashCountedSet.h>
#include <cstring>
#include <cstddef>

#define KJS_MEM_LIMIT 500000

namespace KJS
{
class JSCell;
class JSValue;
class CollectorBlock;

/**
 * @short Garbage collector.
 */
class KJS_EXPORT Collector
{
    // disallow direct construction/destruction
    Collector();
public:
    /**
     * Register an object with the collector. The following assumptions are
     * made:
     * @li the operator new() of the object class is overloaded.
     * @li operator delete() has been overloaded as well and does not free
     * the memory on its own.
     *
     * @param s Size of the memory to be registered.
     * @return A pointer to the allocated memory.
     */
    static void *allocate(size_t s);
    /**
     * Run the garbage collection. This involves calling the delete operator
     * on each object and freeing the used memory.
     */
    static bool collect();

    static const size_t minExtraCostSize = 256;

    static void reportExtraMemoryCost(size_t cost);

    static size_t size();
    static bool isOutOfMemory()
    {
        return memoryFull;
    }

#ifdef KJS_DEBUG_MEM
    /**
     * Check that nothing is left when the last interpreter gets deleted
     */
    static void finalCheck();
#endif

    static void protect(JSValue *);
    static void unprotect(JSValue *);

    static size_t numInterpreters();
    static size_t numProtectedObjects();
    static HashCountedSet<const char *> *rootObjectTypeCounts();

    class Thread;
    static void registerThread();

    static bool isCellMarked(const JSCell *);
    static void markCell(JSCell *);

private:
    static const CollectorBlock *cellBlock(const JSCell *);
    static CollectorBlock *cellBlock(JSCell *);
    static size_t cellOffset(const JSCell *);

    static void recordExtraCost(size_t);
    static void markProtectedObjects();
    static void markCurrentThreadConservatively();
    static void markOtherThreadConservatively(Thread *);
    static void markStackObjectsConservatively();
    static void markStackObjectsConservatively(void *start, void *end);

    static bool memoryFull;
    static void reportOutOfMemoryToAllInterpreters();
};

// tunable parameters
template<size_t bytesPerWord> struct CellSize;

// cell size needs to be a power of two for certain optimizations in collector.cpp
// below also assume it's divisible by 8, and that block size is divisible by it
template<> struct CellSize<4> {
    static const size_t m_value = 32;
}; // 32-bit
template<> struct CellSize<8> {
    static const size_t m_value = 64;
}; // 64-bit
const size_t BLOCK_SIZE = 16 * 4096; // 64k

const size_t BLOCK_OFFSET_MASK = BLOCK_SIZE - 1;
const size_t BLOCK_MASK = ~BLOCK_OFFSET_MASK;

const size_t CELL_SIZE = CellSize<sizeof(void *)>::m_value;
const size_t CELL_ARRAY_LENGTH = (CELL_SIZE / sizeof(double));
const size_t CELL_MASK = CELL_SIZE - 1;

// For each block, we can have at /most/ BLOCK_SIZE/CELL_SIZE entries.
// Sice the bitmap accordingly, don't try to be fancy
const size_t BITMAP_SIZE = (BLOCK_SIZE / CELL_SIZE + 7) / 8;
const size_t BITMAP_WORDS = (BITMAP_SIZE + 3) / sizeof(uint32_t);

// In each block, we have 3 bitmaps (mark for all blocks, extension + allocd for oversize cell blocks),
// as well as an int and a pointer
const size_t BLOCK_METADATA_SIZE = 3 * 4 * BITMAP_WORDS + sizeof(uint32_t) + sizeof(void *);
const size_t CELLS_PER_BLOCK     = (BLOCK_SIZE - BLOCK_METADATA_SIZE) / CELL_SIZE;

struct CollectorBitmap {
    uint32_t bits[BITMAP_WORDS];
    bool get(size_t n) const
    {
        return !!(bits[n >> 5] & (1 << (n & 0x1F)));
    }
    void set(size_t n)
    {
        bits[n >> 5] |= (1 << (n & 0x1F));
    }
    void clear(size_t n)
    {
        bits[n >> 5] &= ~(1 << (n & 0x1F));
    }
    void clearAll()
    {
        std::memset(bits, 0, sizeof(bits));
    }
};

struct CollectorCell {
    union {
        double memory[CELL_ARRAY_LENGTH];
        struct {
            void *zeroIfFree;
            ptrdiff_t next;
        } freeCell;
    } u;
};

class CollectorBlock
{
public:
    CollectorCell cells[CELLS_PER_BLOCK];
    uint32_t usedCells;
    CollectorCell *freeList;
    CollectorBitmap marked;
    CollectorBitmap allocd;
    CollectorBitmap trailer;
};

inline const CollectorBlock *Collector::cellBlock(const JSCell *cell)
{
    return reinterpret_cast<const CollectorBlock *>(reinterpret_cast<uintptr_t>(cell) & BLOCK_MASK);
}

inline CollectorBlock *Collector::cellBlock(JSCell *cell)
{
    return const_cast<CollectorBlock *>(cellBlock(const_cast<const JSCell *>(cell)));
}

inline size_t Collector::cellOffset(const JSCell *cell)
{
    return (reinterpret_cast<uintptr_t>(cell) & BLOCK_OFFSET_MASK) / CELL_SIZE;
}

inline bool Collector::isCellMarked(const JSCell *cell)
{
    return cellBlock(cell)->marked.get(cellOffset(cell));
}

inline void Collector::markCell(JSCell *cell)
{
    cellBlock(cell)->marked.set(cellOffset(cell));
}

inline void Collector::reportExtraMemoryCost(size_t cost)
{
    if (cost > minExtraCostSize) {
        recordExtraCost(cost / (CELL_SIZE * 2));
    }
}
}

#endif /* _KJSCOLLECTOR_H_ */
