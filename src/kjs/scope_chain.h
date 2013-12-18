/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2003 Apple Computer, Inc.
 *            (C) 2008 Maksim Orlovich <maksim@kde.org>
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

#ifndef KJS_SCOPE_CHAIN_H
#define KJS_SCOPE_CHAIN_H

#include "global.h"
#if HAVE_STDINT_H
#include <stdint.h>
#endif
#include <assert.h>

namespace KJS
{

class JSObject;
class JSVariableObject;
class ScopeChainNode;

class KJS_EXPORT ScopeChainLink
{
public:
    /* This class abstracts a link to a node in a scope chain. The node may be
        either a JSVariable, which stores the next pointer in one of its
        slots, or a full-blown ScopeChainNode with separate object/next field and a
        refcount.

        We link to variable object by storing the pointer to them; to ScopeChainNode's by
        storing the pointer with lower bit set.
    */
    uintptr_t ptr;

    // in lieu of constructor, for POD'ness
    void init()
    {
        ptr = 0;
    }

    void set(ScopeChainNode *node)
    {
        ptr = reinterpret_cast<uintptr_t>(node) | 1;
    }

    void set(JSVariableObject *act)
    {
        ptr = reinterpret_cast<uintptr_t>(act);
    }

    // these are inline in JSVariableObject.h
    JSObject      *object() const;
    ScopeChainLink next()   const;

    void deref();
    void ref();

    bool isToScopeChainNode() const
    {
        return ptr & 1;
    }

    JSVariableObject *asVariableObject() const
    {
        assert(!isToScopeChainNode());
        return reinterpret_cast<JSVariableObject *>(ptr);
    }

    ScopeChainNode *asScopeChainNode() const
    {
        assert(isToScopeChainNode());
        return reinterpret_cast<ScopeChainNode *>(ptr & ~1);
    }

    bool operator == (const ScopeChainLink &other) const
    {
        return other.ptr == ptr;
    }

    bool operator != (const ScopeChainLink &other) const
    {
        return other.ptr != ptr;
    }
};

// A ScopeChainNode is used to put things other than JSVariableObject's
// in the scope chain.
class KJS_EXPORT ScopeChainNode
{
public:
    ScopeChainNode(ScopeChainLink n, JSObject *o)
        : next(n), object(o), refCount(1)
    {
        // Note: we don't ref the next here, since its reference is
        // transferred from top pointer in ScopeChain to us
        // We ourselves are ref'd by the refCount(1)
    }

    ~ScopeChainNode()
    {
        // ### non-recursive?
        next.deref();
    }

    ScopeChainLink next;
    JSObject *object;
    int refCount;

    void ref()
    {
        ++refCount;
    }
    void deref()
    {
        --refCount;
        if (!refCount) {
            delete this;
        }
    }
};

class KJS_EXPORT ScopeChainIterator
{
public:
    ScopeChainIterator(ScopeChainLink node) : m_node(node) {}

    JSObject *operator*()  const
    {
        return m_node.object();
    }

    ScopeChainIterator &operator++()
    {
        m_node = m_node.next();
        return *this;
    }

    // postfix ++ intentionally omitted

    bool operator==(const ScopeChainIterator &other) const
    {
        return m_node == other.m_node;
    }
    bool operator!=(const ScopeChainIterator &other) const
    {
        return m_node != other.m_node;
    }

private:
    ScopeChainLink m_node;
};

class KJS_EXPORT ScopeChain
{
public:
    ScopeChain()
    {
        m_top.init();
    }

    ~ScopeChain()
    {
        m_top.deref();
    }

    ScopeChain(const ScopeChain &c) : m_top(c.m_top)
    {
        m_top.ref();
    }

    ScopeChain &operator=(const ScopeChain &);

    JSObject *top() const
    {
        return m_top.object();
    }
    JSObject *bottom() const;

    ScopeChainIterator begin() const;
    ScopeChainIterator end() const;

    void push(JSObject *obj)
    {
        m_top.set(new ScopeChainNode(m_top, obj));
    }

    // inline def in JSVariableObject.h
    void pushVariableObject(JSVariableObject *act);

    void pop();
    void mark(); // inline in JSVariableObject.h

#ifndef NDEBUG
    void print();
#endif

private:
    ScopeChainLink m_top;
};

inline void ScopeChainLink::deref()
{
    if (isToScopeChainNode()) {
        asScopeChainNode()->deref();
    }
    ptr = 0;
}

inline void ScopeChainLink::ref()
{
    if (isToScopeChainNode()) {
        asScopeChainNode()->ref();
    }
}

inline ScopeChainIterator ScopeChain::begin() const
{
    return ScopeChainIterator(m_top);
}

inline ScopeChainIterator ScopeChain::end() const
{
    ScopeChainLink empty;
    empty.init();
    return ScopeChainIterator(empty);
}

inline ScopeChain &ScopeChain::operator=(const ScopeChain &c)
{
    ScopeChainLink newTop = c.m_top;
    newTop.ref();
    m_top.deref();
    m_top = newTop;
    return *this;
}

inline JSObject *ScopeChain::bottom() const
{
    ScopeChainLink last;
    for (ScopeChainLink n = m_top; n.ptr; n = n.next()) {
        last = n;
    }
    return last.object();
}

inline void ScopeChain::pop()
{
    // ### may need manual CSE, it also thrashes more that needed
    ScopeChainLink newTop = m_top.next();
    newTop.ref();
    m_top.deref();
    m_top = newTop;
}

} // namespace KJS

#endif // KJS_SCOPE_CHAIN_H
