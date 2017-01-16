/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2003, 2004, 2005, 2006, 2007 Apple Computer, Inc.
 *  Copyright (C) 2007 Eric Seidel <eric@webkit.org>
 *  Copyright (C) 2007 Maksim Orlovich <maksim@kde.org>
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

#include "collector.h"

#include <wtf/FastMalloc.h>
#include <wtf/HashCountedSet.h>
#include "internal.h"
#include "list.h"
#include "value.h"

#include <setjmp.h>
#include <limits.h>
#include <algorithm>

#if PLATFORM(DARWIN)

#include <pthread.h>
#include <mach/mach_port.h>
#include <mach/mach_init.h>
#include <mach/task.h>
#include <mach/thread_act.h>
#include <mach/vm_map.h>

#elif PLATFORM(WIN_OS) || COMPILER(CYGWIN)

#include <windows.h>
#include <winnt.h>

#elif PLATFORM(UNIX)

#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <pthread.h> //krazy:exclude=includes (yes, it's duplicate, but in a different #if branch)
#include <unistd.h>

#if PLATFORM(SOLARIS_OS)
#include <thread.h>
#include <signal.h>
using std::memset;
#endif

#if HAVE_PTHREAD_NP_H
#include <pthread_np.h>
#endif

#endif

#define DEBUG_COLLECTOR 0

#if HAVE_VALGRIND_MEMCHECK_H && !defined(NDEBUG)
#include <valgrind/memcheck.h>
#if defined(VALGRIND_MAKE_MEM_DEFINED)
#define VG_DEFINED(p) VALGRIND_MAKE_MEM_DEFINED(&p, sizeof(void*))
#else
#define VG_DEFINED(p)
#endif
#else
#define VG_DEFINED(p)
#endif

using std::max;

namespace KJS
{

// tunable parameters
const size_t SPARE_EMPTY_BLOCKS = 2;
const size_t MIN_ARRAY_SIZE = 14;
const size_t GROWTH_FACTOR = 2;
const size_t LOW_WATER_FACTOR = 4;
const size_t ALLOCATIONS_PER_COLLECTION = 4000;

// A whee bit like a WTF::Vector, but perfectly POD, so can be used in global context
// w/o worries.
struct BlockList {
    CollectorBlock **m_data;
    size_t m_used;
    size_t m_capacity;

    CollectorBlock *operator[](size_t pos)
    {
        return m_data[pos];
    }

    size_t used() const
    {
        return m_used;
    }

    void append(CollectorBlock *block)
    {
        if (m_used == m_capacity) {
            static const size_t maxNumBlocks = ULONG_MAX / sizeof(CollectorBlock *) / GROWTH_FACTOR;
            if (m_capacity > maxNumBlocks) {
                CRASH();
            }
            m_capacity = max(MIN_ARRAY_SIZE, m_capacity * GROWTH_FACTOR);
            m_data = static_cast<CollectorBlock **>(fastRealloc(m_data, m_capacity * sizeof(CollectorBlock *)));
        }
        m_data[m_used] = block;
        ++m_used;
    }

    void remove(CollectorBlock *block)
    {
        size_t c;
        for (c = 0; c < m_used; ++c)
            if (m_data[c] == block) {
                break;
            }

        if (c == m_used) {
            return;
        }

        // Move things up, and shrink..
        --m_used;
        for (; c < m_used; ++c) {
            m_data[c] = m_data[c + 1];
        }
    }
};

struct CollectorHeap {
    // This contains the list of both normal and oversize blocks
    BlockList allBlocks;

    // Just the normal blocks
    BlockList blocks;

    size_t firstBlockWithPossibleSpace;

    // The oversize block handling is a bit tricky, since we do not wish to slow down
    // the usual paths. Hence, we do the following:
    // 1) We set the marked bits for any extension portions of the block.
    //    this way the stack GC doesn't have to do anything special if a pointer
    //    happens to go that way.
    // 2) We keep track of an allBlocks list to help the stack GC tests things.
    //
    // The oversize heap itself represents blocks as follows:
    // 1) free: marked = false, allocd = false, trailer = false, zeroIfFree = 0
    // 2) alloc'd, head: marked = depends, allocd = true, trailer = false, zeroIfFree is uncertain
    // 3) alloc'd, trailer: marked = true (see above), allocd = true, trailer = true, zeroIfFree is uncertain
    //
    // For now, we don't use a freelist, so performance may be quite bad if
    // this is used heavily; this is just because the cell does not have
    // back and forward links; which we need since we can pick a non-first cell
    // ### actually, it may be possible to shuffle the list. Not now, though
    BlockList oversizeBlocks;

    size_t numLiveObjects;
    size_t numLiveObjectsAtLastCollect;
    size_t extraCost;
};

static CollectorHeap heap;

bool Collector::memoryFull = false;

static CollectorBlock *allocateBlock()
{
#if PLATFORM(DARWIN)
    vm_address_t address = 0;
    vm_map(current_task(), &address, BLOCK_SIZE, BLOCK_OFFSET_MASK, VM_FLAGS_ANYWHERE, MEMORY_OBJECT_NULL, 0, FALSE, VM_PROT_DEFAULT, VM_PROT_DEFAULT, VM_INHERIT_DEFAULT);
#elif PLATFORM(WIN_OS) || COMPILER(CYGWIN)
    // windows virtual address granularity is naturally 64k
    LPVOID address = VirtualAlloc(NULL, BLOCK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#elif HAVE_FUNC_POSIX_MEMALIGN
    void *address;
    posix_memalign(&address, BLOCK_SIZE, BLOCK_SIZE);
    memset(reinterpret_cast<void *>(address), 0, BLOCK_SIZE);
#else
    static size_t pagesize = getpagesize();

    size_t extra = 0;
    if (BLOCK_SIZE > pagesize) {
        extra = BLOCK_SIZE - pagesize;
    }

    void *mmapResult = mmap(NULL, BLOCK_SIZE + extra, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    uintptr_t address = reinterpret_cast<uintptr_t>(mmapResult);

    size_t adjust = 0;
    if ((address & BLOCK_OFFSET_MASK) != 0) {
        adjust = BLOCK_SIZE - (address & BLOCK_OFFSET_MASK);
    }

    if (adjust > 0) {
        munmap(reinterpret_cast<void *>(address), adjust);
    }

    if (adjust < extra) {
        munmap(reinterpret_cast<void *>(address + adjust + BLOCK_SIZE), extra - adjust);
    }

    address += adjust;
    memset(reinterpret_cast<void *>(address), 0, BLOCK_SIZE);
#endif
    CollectorBlock *ptr = reinterpret_cast<CollectorBlock *>(address);
    heap.allBlocks.append(ptr); // Register with the heap
    return ptr;
}

static void freeBlock(CollectorBlock *block)
{
    // Unregister the block first
    heap.allBlocks.remove(block);

#if PLATFORM(DARWIN)
    vm_deallocate(current_task(), reinterpret_cast<vm_address_t>(block), BLOCK_SIZE);
#elif PLATFORM(WIN_OS) || COMPILER(CYGWIN)
    VirtualFree(block, BLOCK_SIZE, MEM_RELEASE);
#elif HAVE_FUNC_POSIX_MEMALIGN
    free(block);
#else
    munmap(block, BLOCK_SIZE);
#endif
}

void Collector::recordExtraCost(size_t cost)
{
    // Our frequency of garbage collection tries to balance memory use against speed
    // by collecting based on the number of newly created values. However, for values
    // that hold on to a great deal of memory that's not in the form of other JS values,
    // that is not good enough - in some cases a lot of those objects can pile up and
    // use crazy amounts of memory without a GC happening. So we track these extra
    // memory costs. Only unusually large objects are noted, and we only keep track
    // of this extra cost until the next GC. In garbage collected languages, most values
    // are either very short lived temporaries, or have extremely long lifetimes. So
    // if a large value survives one garbage collection, there is not much point to
    // collecting more frequently as long as it stays alive.

    heap.extraCost += cost;
}

static void *allocOversize(size_t s)
{
    size_t cellsNeeded = (s + (CELL_SIZE - 1)) / CELL_SIZE;

    // printf("allocOversize, size:%d, cellsNeeded:%d\n", s, cellsNeeded);

    // We are not oversize enough to deal with things close to 64K in size
    assert(cellsNeeded <= CELLS_PER_BLOCK);

    // Look through the blocks, see if one has enough, and where.
    CollectorBlock *sufficientBlock = nullptr;
    size_t startOffset = -1;
    for (size_t b = 0; b < heap.oversizeBlocks.used() && !sufficientBlock; ++b) {
        CollectorBlock *candidate = heap.oversizeBlocks[b];
        if (cellsNeeded <= CELLS_PER_BLOCK - candidate->usedCells) {
            // Well, there is a chance we will fit.. Let's see if we can find a batch long enough..
            for (size_t i = 0; i < CELLS_PER_BLOCK; i++) {
                if (i % 32 == 0 && candidate->allocd.bits[i / 32] == 0xFFFFFFFF) {
                    // Can skip this and 31 other cells
                    i += 31;
                    continue;
                }

                if (candidate->allocd.get(i)) {
                    continue;
                }

                // This cell is free -- are there enough free cells after it?
                startOffset = i;
                size_t last = i + cellsNeeded - 1;

                if (last >= CELLS_PER_BLOCK) { // No way it will fit
                    break;
                }

                ++i;
                while (i <= last && !candidate->allocd.get(i)) {
                    ++i;
                }

                // Did we get through enough?
                if (i == last + 1) {
                    sufficientBlock = candidate;
                    break;
                }

                // Not enough room -- and the current entry has a non-zero zeroIfFree,
                // so we should go on at i + 1 on next iteration

            } // for each cell per block.
        } // if enough room in block
    } // for each block

    if (!sufficientBlock) {
        sufficientBlock = allocateBlock();
        startOffset     = 0;
        heap.oversizeBlocks.append(sufficientBlock);
    }

    sufficientBlock->usedCells += cellsNeeded;

    // Set proper bits for trailers and the head
    sufficientBlock->allocd.set(startOffset);
    for (size_t t = startOffset + 1; t < startOffset + cellsNeeded; ++t) {
        sufficientBlock->trailer.set(t);
        sufficientBlock->marked.set(t);
        sufficientBlock->allocd.set(t);
    }

    void *result = sufficientBlock->cells + startOffset;
    memset(result, 0, s);
    heap.numLiveObjects = heap.numLiveObjects + 1;
    return result;
}

void *Collector::allocate(size_t s)
{
    assert(JSLock::lockCount() > 0);

    // collect if needed
    size_t numLiveObjects = heap.numLiveObjects;
    size_t numLiveObjectsAtLastCollect = heap.numLiveObjectsAtLastCollect;
    size_t numNewObjects = numLiveObjects - numLiveObjectsAtLastCollect;
    size_t newCost = numNewObjects + heap.extraCost;

    if (newCost >= ALLOCATIONS_PER_COLLECTION && newCost >= numLiveObjectsAtLastCollect) {
        collect();
        numLiveObjects = heap.numLiveObjects;
    }

    if (s > CELL_SIZE) {
        return allocOversize(s);
    }

    // slab allocator

    size_t usedBlocks = heap.blocks.used();

    size_t i = heap.firstBlockWithPossibleSpace;
    CollectorBlock *targetBlock;
    size_t targetBlockUsedCells;
    if (i != usedBlocks) {
        targetBlock = heap.blocks[i];
        targetBlockUsedCells = targetBlock->usedCells;
        assert(targetBlockUsedCells <= CELLS_PER_BLOCK);
        while (targetBlockUsedCells == CELLS_PER_BLOCK) {
            if (++i == usedBlocks) {
                goto allocateNewBlock;
            }
            targetBlock = heap.blocks[i];
            targetBlockUsedCells = targetBlock->usedCells;
            assert(targetBlockUsedCells <= CELLS_PER_BLOCK);
        }
        heap.firstBlockWithPossibleSpace = i;
    } else {
    allocateNewBlock:
        // didn't find one, need to allocate a new block
        targetBlock = allocateBlock();
        targetBlock->freeList = targetBlock->cells;
        targetBlockUsedCells = 0;
        heap.blocks.append(targetBlock);
        heap.firstBlockWithPossibleSpace = usedBlocks; // previous usedBlocks -> new one's index
    }

    // find a free spot in the block and detach it from the free list
    CollectorCell *newCell = targetBlock->freeList;

    // "next" field is a byte offset -- 0 means next cell, so a zeroed block is already initialized
    // could avoid the casts by using a cell offset, but this avoids a relatively-slow multiply
    targetBlock->freeList = reinterpret_cast<CollectorCell *>(reinterpret_cast<char *>(newCell + 1) + newCell->u.freeCell.next);

    targetBlock->usedCells = targetBlockUsedCells + 1;
    heap.numLiveObjects = numLiveObjects + 1;

    return newCell;
}

#if USE(MULTIPLE_THREADS)

struct Collector::Thread {
    Thread(pthread_t pthread, mach_port_t mthread) : posixThread(pthread), machThread(mthread) {}
    Thread *next;
    pthread_t posixThread;
    mach_port_t machThread;
};

pthread_key_t registeredThreadKey;
pthread_once_t registeredThreadKeyOnce = PTHREAD_ONCE_INIT;

Collector::Thread *registeredThreads;

static void destroyRegisteredThread(void *data)
{
    Collector::Thread *thread = (Collector::Thread *)data;

    if (registeredThreads == thread) {
        registeredThreads = registeredThreads->next;
    } else {
        Collector::Thread *last = registeredThreads;
        for (Collector::Thread *t = registeredThreads->next; t != NULL; t = t->next) {
            if (t == thread) {
                last->next = t->next;
                break;
            }
            last = t;
        }
    }

    delete thread;
}

static void initializeRegisteredThreadKey()
{
    pthread_key_create(&registeredThreadKey, destroyRegisteredThread);
}

void Collector::registerThread()
{
    pthread_once(&registeredThreadKeyOnce, initializeRegisteredThreadKey);

    if (!pthread_getspecific(registeredThreadKey)) {
        pthread_t pthread = pthread_self();
        WTF::fastMallocRegisterThread(pthread);
        Collector::Thread *thread = new Collector::Thread(pthread, pthread_mach_thread_np(pthread));
        thread->next = registeredThreads;
        registeredThreads = thread;
        pthread_setspecific(registeredThreadKey, thread);
    }
}

#endif

#define IS_POINTER_ALIGNED(p) (((intptr_t)(p) & (sizeof(char *) - 1)) == 0)

// cell size needs to be a power of two for this to be valid
#define IS_CELL_ALIGNED(p) (((intptr_t)(p) & CELL_MASK) == 0)

void Collector::markStackObjectsConservatively(void *start, void *end)
{
    if (start > end) {
        void *tmp = start;
        start = end;
        end = tmp;
    }

    assert(((char *)end - (char *)start) < 0x1000000);
    assert(IS_POINTER_ALIGNED(start));
    assert(IS_POINTER_ALIGNED(end));

    char **p = (char **)start;
    char **e = (char **)end;

    // We use allBlocks here since we want to mark oversize cells as well.
    // Their trailers will have the mark bit set already, to avoid trouble.
    size_t usedBlocks = heap.allBlocks.used();
    CollectorBlock **blocks = heap.allBlocks.m_data;

    const size_t lastCellOffset = sizeof(CollectorCell) * (CELLS_PER_BLOCK - 1);

    while (p != e) {
        char *x = *p++;
        VG_DEFINED(x);
        if (IS_CELL_ALIGNED(x) && x) {
            uintptr_t offset = reinterpret_cast<uintptr_t>(x) & BLOCK_OFFSET_MASK;
            CollectorBlock *blockAddr = reinterpret_cast<CollectorBlock *>(x - offset);
            for (size_t block = 0; block < usedBlocks; block++) {
                if ((blocks[block] == blockAddr) && (offset <= lastCellOffset)) {
                    if (((CollectorCell *)x)->u.freeCell.zeroIfFree != nullptr) {
                        JSCell *imp = reinterpret_cast<JSCell *>(x);
                        if (!imp->marked()) {
                            imp->mark();
                        }
                    }
                } // if valid block
            } // for each block
        } // if cell-aligned
    } // for each pointer
}

static inline void *currentThreadStackBase()
{
#if PLATFORM(DARWIN)
    pthread_t thread = pthread_self();
    void *stackBase = pthread_get_stackaddr_np(thread);
#elif (PLATFORM(WIN_OS) || COMPILER(CYGWIN))
    // tested with mingw32, mingw64, msvc2008, cygwin
    NT_TIB *pTib = (NT_TIB *)NtCurrentTeb();
    void *stackBase = (void *)pTib->StackBase;
#elif PLATFORM(SOLARIS_OS)
    stack_t s;
    thr_stksegment(&s);
    return s.ss_sp;
    // NOTREACHED
    void *stackBase = 0;
#elif PLATFORM(UNIX)
    static void *stackBase = nullptr;
    static pthread_t stackThread;
    pthread_t thread = pthread_self();
    if (stackBase == nullptr || thread != stackThread) {
        pthread_attr_t sattr;
#if HAVE_PTHREAD_NP_H || defined(__NetBSD__)
        // e.g. on FreeBSD 5.4, neundorf@kde.org
        // also on NetBSD 3 and 4, raphael.langerhorst@kdemail.net
        // HIGHLY RECCOMENDED by manpage to allocate storage, avoids
        // crashing in JS immediately in FreeBSD.
        pthread_attr_init(&sattr);
        pthread_attr_get_np(thread, &sattr);
#else
        // FIXME: this function is non-portable; other POSIX systems may have different np alternatives
        pthread_getattr_np(thread, &sattr);
#endif // picking the _np function to use --- Linux or BSD

        size_t stackSize;
        pthread_attr_getstack(&sattr, &stackBase, &stackSize);
        stackBase = (char *)stackBase + stackSize;      // a matter of interpretation, apparently...
        pthread_attr_destroy(&sattr);
        assert(stackBase);
        stackThread = thread;
    }
#else
#error Need a way to get the stack base on this platform
#endif
    return stackBase;
}

void Collector::markCurrentThreadConservatively()
{
    // setjmp forces volatile registers onto the stack
    jmp_buf registers;
#if COMPILER(MSVC)
#pragma warning(push)
#pragma warning(disable: 4611)
#endif
    setjmp(registers);
#if COMPILER(MSVC)
#pragma warning(pop)
#endif

    void *dummy;
    void *stackPointer = &dummy;
    void *stackBase = currentThreadStackBase();

    markStackObjectsConservatively(stackPointer, stackBase);
}

#if USE(MULTIPLE_THREADS)

typedef unsigned long usword_t; // word size, assumed to be either 32 or 64 bit

void Collector::markOtherThreadConservatively(Thread *thread)
{
    thread_suspend(thread->machThread);

#if PLATFORM(X86)
    i386_thread_state_t regs;
    unsigned user_count = sizeof(regs) / sizeof(int);
    thread_state_flavor_t flavor = i386_THREAD_STATE;
#elif PLATFORM(X86_64)
    x86_thread_state64_t  regs;
    unsigned user_count = x86_THREAD_STATE64_COUNT;
    thread_state_flavor_t flavor = x86_THREAD_STATE64;
#elif PLATFORM(PPC)
    ppc_thread_state_t  regs;
    unsigned user_count = PPC_THREAD_STATE_COUNT;
    thread_state_flavor_t flavor = PPC_THREAD_STATE;
#elif PLATFORM(PPC64)
    ppc_thread_state64_t  regs;
    unsigned user_count = PPC_THREAD_STATE64_COUNT;
    thread_state_flavor_t flavor = PPC_THREAD_STATE64;
#else
#error Unknown Architecture
#endif
    // get the thread register state
    thread_get_state(thread->machThread, flavor, (thread_state_t)&regs, &user_count);

    // scan the registers
    markStackObjectsConservatively((void *)&regs, (void *)((char *)&regs + (user_count * sizeof(usword_t))));

    // scan the stack
#if PLATFORM(X86) && __DARWIN_UNIX03
    markStackObjectsConservatively((void *)regs.__esp, pthread_get_stackaddr_np(thread->posixThread));
#elif PLATFORM(X86)
    markStackObjectsConservatively((void *)regs.esp, pthread_get_stackaddr_np(thread->posixThread));
#elif PLATFORM(X86_64) && __DARWIN_UNIX03
    markStackObjectsConservatively((void *)regs.__rsp, pthread_get_stackaddr_np(thread->posixThread));
#elif PLATFORM(X86_64)
    markStackObjectsConservatively((void *)regs.rsp, pthread_get_stackaddr_np(thread->posixThread));
#elif (PLATFORM(PPC) || PLATFORM(PPC64)) && __DARWIN_UNIX03
    markStackObjectsConservatively((void *)regs.__r1, pthread_get_stackaddr_np(thread->posixThread));
#elif PLATFORM(PPC) || PLATFORM(PPC64)
    markStackObjectsConservatively((void *)regs.r1, pthread_get_stackaddr_np(thread->posixThread));
#else
#error Unknown Architecture
#endif

    thread_resume(thread->machThread);
}

#endif

void Collector::markStackObjectsConservatively()
{
    markCurrentThreadConservatively();

#if USE(MULTIPLE_THREADS)
    for (Thread *thread = registeredThreads; thread != NULL; thread = thread->next) {
        if (thread->posixThread != pthread_self()) {
            markOtherThreadConservatively(thread);
        }
    }
#endif
}

typedef HashCountedSet<JSCell *> ProtectCounts;

static ProtectCounts &protectedValues()
{
    static ProtectCounts pv;
    return pv;
}

void Collector::protect(JSValue *k)
{
    assert(k);
    assert(JSLock::lockCount() > 0);

    if (JSImmediate::isImmediate(k)) {
        return;
    }

    protectedValues().add(k->asCell());
}

void Collector::unprotect(JSValue *k)
{
    assert(k);
    assert(JSLock::lockCount() > 0);

    if (JSImmediate::isImmediate(k)) {
        return;
    }

    protectedValues().remove(k->asCell());
}

void Collector::markProtectedObjects()
{
    ProtectCounts &pv = protectedValues();
    ProtectCounts::iterator end = pv.end();
    for (ProtectCounts::iterator it = pv.begin(); it != end; ++it) {
        JSCell *val = it->first;
        if (!val->marked()) {
            val->mark();
        }
    }
}

bool Collector::collect()
{
    assert(JSLock::lockCount() > 0);

#if USE(MULTIPLE_THREADS)
    bool currentThreadIsMainThread = pthread_main_np();
#else
    bool currentThreadIsMainThread = true;
#endif

    Interpreter::markSourceCachedObjects();

    if (Interpreter::s_hook) {
        Interpreter *scr = Interpreter::s_hook;
        do {
            scr->mark(currentThreadIsMainThread);
            scr = scr->next;
        } while (scr != Interpreter::s_hook);
    }

    // MARK: first mark all referenced objects recursively starting out from the set of root objects

    markStackObjectsConservatively();
    markProtectedObjects();
    List::markProtectedLists();
#if USE(MULTIPLE_THREADS)
    if (!currentThreadIsMainThread) {
        markMainThreadOnlyObjects();
    }
#endif

    // SWEEP: delete everything with a zero refcount (garbage) and unmark everything else

    // Note: if a cell has zeroIfFree == 0, it is either free,
    // or in the middle of being constructed and has not yet
    // had its vtable filled. Hence, such cells should not be cleaned up

    size_t emptyBlocks = 0;
    size_t numLiveObjects = heap.numLiveObjects;

    for (size_t block = 0; block < heap.blocks.used(); block++) {
        CollectorBlock *curBlock = heap.blocks[block];

        size_t usedCells = curBlock->usedCells;
        CollectorCell *freeList = curBlock->freeList;

        if (usedCells == CELLS_PER_BLOCK) {
            // special case with a block where all cells are used -- testing indicates this happens often
            for (size_t i = 0; i < CELLS_PER_BLOCK; i++) {
                CollectorCell *cell = curBlock->cells + i;
                JSCell *imp = reinterpret_cast<JSCell *>(cell);
                if (!curBlock->marked.get(i) && currentThreadIsMainThread) {
                    // special case for allocated but uninitialized object
                    // (We don't need this check earlier because nothing prior this point assumes the object has a valid vptr.)
                    if (cell->u.freeCell.zeroIfFree == nullptr) {
                        continue;
                    }
                    imp->~JSCell();
                    --usedCells;
                    --numLiveObjects;

                    // put cell on the free list
                    cell->u.freeCell.zeroIfFree = nullptr;
                    cell->u.freeCell.next = reinterpret_cast<char *>(freeList) - reinterpret_cast<char *>(cell + 1);
                    freeList = cell;
                }
            }
        } else {
            size_t minimumCellsToProcess = usedCells;
            for (size_t i = 0; (i < minimumCellsToProcess) & (i < CELLS_PER_BLOCK); i++) {
                CollectorCell *cell = curBlock->cells + i;
                if (cell->u.freeCell.zeroIfFree == nullptr) {
                    ++minimumCellsToProcess;
                } else {
                    JSCell *imp = reinterpret_cast<JSCell *>(cell);
                    if (!curBlock->marked.get(i) && currentThreadIsMainThread) {
                        imp->~JSCell();
                        --usedCells;
                        --numLiveObjects;

                        // put cell on the free list
                        cell->u.freeCell.zeroIfFree = nullptr;
                        cell->u.freeCell.next = reinterpret_cast<char *>(freeList) - reinterpret_cast<char *>(cell + 1);
                        freeList = cell;
                    }
                }
            }
        }

        curBlock->marked.clearAll();
        curBlock->usedCells = usedCells;
        curBlock->freeList = freeList;

        if (usedCells == 0) {
            emptyBlocks++;
            if (emptyBlocks > SPARE_EMPTY_BLOCKS) {
#if !DEBUG_COLLECTOR
                freeBlock(curBlock);
#endif
                // swap with the last block so we compact as we go
                heap.blocks.m_data[block] = heap.blocks[heap.blocks.used() - 1];
                heap.blocks.m_used--;
                block--; // Don't move forward a step in this case
            }
        }
    }

    if (heap.numLiveObjects != numLiveObjects) {
        heap.firstBlockWithPossibleSpace = 0;
    }

    // Now sweep oversize blocks.
    emptyBlocks = 0;
    for (size_t ob = 0; ob < heap.oversizeBlocks.used(); ++ob) {
        CollectorBlock *curBlock = heap.oversizeBlocks[ob];
        CollectorCell *freeList = curBlock->freeList;
        size_t usedCells = curBlock->usedCells;

        // Go through the cells
        for (size_t i = 0; i < CELLS_PER_BLOCK; i++) {
            if (i % 32 == 0 && curBlock->allocd.bits[i / 32] == 0) {
                // Nothing used around here, skip this and 31 next cells
                i += 31;
                continue;
            }

            CollectorCell *cell = curBlock->cells + i;
            if (cell->u.freeCell.zeroIfFree == nullptr) {
                continue;
            }

            if (!curBlock->allocd.get(i)) {
                continue;
            }

            JSCell *imp = reinterpret_cast<JSCell *>(cell);

            // Live and trailer cells will all have the mark set,
            // so we only have to collect with it clear --
            // and we already took care of those that are
            // already free (or being initialized) above
            if (!curBlock->marked.get(i)) {
                // Free this block...
                imp->~JSCell();
                --numLiveObjects;
                --usedCells;

                // Mark the block and the trailers as available
                cell->u.freeCell.zeroIfFree = nullptr;
                curBlock->allocd.clear(i);

                ++i; // Go to the potential trailer..
                while (curBlock->trailer.get(i) && i < CELLS_PER_BLOCK) {
                    --usedCells;
                    curBlock->allocd.clear(i);
                    curBlock->trailer.clear(i);
                    curBlock->marked.clear(i);
                    curBlock->cells[i].u.freeCell.zeroIfFree = nullptr;
                    ++i;
                }
                --i; // Last item we processed.
            } else {
                // If this is not a trailer cell, clear the mark
                if (!curBlock->trailer.get(i)) {
                    curBlock->marked.clear(i);
                }
            }
        } // each cell
        curBlock->usedCells = usedCells;
        curBlock->freeList = freeList;
        if (usedCells == 0) {
            emptyBlocks++;
            if (emptyBlocks > SPARE_EMPTY_BLOCKS) {
                freeBlock(curBlock);

                // swap with the last block so we compact as we go
                heap.oversizeBlocks.m_data[ob] = heap.oversizeBlocks[heap.oversizeBlocks.used() - 1];
                heap.oversizeBlocks.m_used--;
                ob--; // Don't move forward a step in this case
            }
        }
    } // each oversize block

    bool deleted = heap.numLiveObjects != numLiveObjects;

    heap.numLiveObjects = numLiveObjects;
    heap.numLiveObjectsAtLastCollect = numLiveObjects;
    heap.extraCost = 0;

    bool newMemoryFull = (numLiveObjects >= KJS_MEM_LIMIT);
    if (newMemoryFull && newMemoryFull != memoryFull) {
        reportOutOfMemoryToAllInterpreters();
    }
    memoryFull = newMemoryFull;

    return deleted;
}

size_t Collector::size()
{
    return heap.numLiveObjects;
}

#ifdef KJS_DEBUG_MEM
void Collector::finalCheck()
{
}
#endif

size_t Collector::numInterpreters()
{
    size_t count = 0;
    if (Interpreter::s_hook) {
        Interpreter *scr = Interpreter::s_hook;
        do {
            ++count;
            scr = scr->next;
        } while (scr != Interpreter::s_hook);
    }
    return count;
}

size_t Collector::numProtectedObjects()
{
    return protectedValues().size();
}

static const char *typeName(JSCell *val)
{
    const char *name = "???";
    switch (val->type()) {
    case UnspecifiedType:
        break;
    case UndefinedType:
        name = "undefined";
        break;
    case NullType:
        name = "null";
        break;
    case BooleanType:
        name = "boolean";
        break;
    case StringType:
        name = "string";
        break;
    case NumberType:
        name = "number";
        break;
    case ObjectType: {
        const ClassInfo *info = static_cast<JSObject *>(val)->classInfo();
        name = info ? info->className : "Object";
        break;
    }
    case GetterSetterType:
        name = "gettersetter";
        break;
    }
    return name;
}

HashCountedSet<const char *> *Collector::rootObjectTypeCounts()
{
    HashCountedSet<const char *> *counts = new HashCountedSet<const char *>;

    ProtectCounts &pv = protectedValues();
    ProtectCounts::iterator end = pv.end();
    for (ProtectCounts::iterator it = pv.begin(); it != end; ++it) {
        counts->add(typeName(it->first));
    }

    return counts;
}

void Collector::reportOutOfMemoryToAllInterpreters()
{
    if (!Interpreter::s_hook) {
        return;
    }

    Interpreter *interpreter = Interpreter::s_hook;
    do {
        ExecState *exec = interpreter->execState();

        exec->setException(Error::create(exec, GeneralError, "Out of memory"));

        interpreter = interpreter->next;
    } while (interpreter != Interpreter::s_hook);
}

} // namespace KJS
