/*
 *  Copyright (C) 2003,2007 Apple Computer, Inc
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

#ifndef KJS_COMMON_IDENTIFIERS_H
#define KJS_COMMON_IDENTIFIERS_H

#include "identifier.h"
#include <wtf/Noncopyable.h>

// List of property names, passed to a macro so we can do set them up various
// ways without repeating the list.
#define KJS_COMMON_IDENTIFIERS_EACH_PROPERTY_NAME(macro) \
    macro(arguments) \
    macro(callee) \
    macro(caller) \
    macro(constructor) \
    macro(fromCharCode) \
    macro(global) \
    macro(ignoreCase) \
    macro(index) \
    macro(input) \
    macro(lastIndex) \
    macro(length) \
    macro(message) \
    macro(multiline) \
    macro(name) \
    macro(prototype) \
    macro(source) \
    macro(toExponential) \
    macro(toFixed) \
    macro(toLocaleString) \
    macro(toPrecision) \
    macro(toString) \
    macro(valueOf) \
    macro(toJSON) \
    macro(configurable) \
    macro(value) \
    macro(writable) \
    macro(enumerable) \
    macro(get) \
    macro(set) \
    macro(toISOString)

namespace KJS {

    class KJS_EXPORT CommonIdentifiers : Noncopyable {

    private:
        CommonIdentifiers();

    public:
        static CommonIdentifiers* shared();

        const Identifier nullIdentifier;
        const Identifier underscoreProto;

#define KJS_IDENTIFIER_DECLARE_PROPERTY_NAME_GLOBAL(name) const Identifier name;
        KJS_COMMON_IDENTIFIERS_EACH_PROPERTY_NAME(KJS_IDENTIFIER_DECLARE_PROPERTY_NAME_GLOBAL)
#undef KJS_IDENTIFIER_DECLARE_PROPERTY_NAME_GLOBAL
    };
} // namespace KJS

// ### better place in identifier.h. only here because of the shared null
namespace WTF {
    // Provide hashing of Identifiers --- using the rep ptr
    struct IdentHash {
        static unsigned hash(const KJS::Identifier& key) {
            return PtrHash<KJS::UString::Rep*>::hash(key.ustring().rep());
        }
        static bool equal(const KJS::Identifier& a, const KJS::Identifier& b) { return a == b; }
        static const bool safeToCompareToEmptyOrDeleted = false;
    };

    template<> struct DefaultHash<KJS::Identifier> { typedef IdentHash Hash; };

    template<> struct HashTraits<KJS::Identifier> : GenericHashTraits<KJS::Identifier> {
        static const bool emptyValueIsZero = false;
        static const bool needsDestruction = true;
        static void constructDeletedValue(KJS::Identifier* slot) { new (slot) KJS::Identifier(); }
        static bool isDeletedValue(const KJS::Identifier& value) { return value.isNull(); }
    };

} // namespace WTF

#endif // KJS_COMMON_IDENTIFIERS_H

