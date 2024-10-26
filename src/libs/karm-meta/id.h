#pragma once

#include <karm-base/base.h>

#include "traits.h"

namespace Karm::Meta {

using Id = usize;

template <typename T>
static constexpr Id idOf() {
    auto cstr = __PRETTY_FUNCTION__;
    auto len = sizeof(__PRETTY_FUNCTION__);
    auto hash = 0uz;
    for (char const *b = cstr; b < cstr + len; b++)
        hash = (1000003 * hash) ^ *b;
    hash ^= len;
    return hash;
}

template <typename T = struct __noType>
struct Type;

template <typename T>
struct Type {
    using TYPE = T;

    constexpr static Id id() {
        return idOf<T>();
    }
};

template <>
struct Type<struct __noType> {
    Id _id;

    template <typename T>
    constexpr Type(Type<T> type)
        : _id(type.id()) {}

    constexpr Id id() const {
        return _id;
    }
};

template <typename T>
Type<T> typeOf() {
    return Type<T>{};
}

} // namespace Karm::Meta
