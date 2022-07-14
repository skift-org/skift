#pragma once

#include <karm-meta/traits.h>

#include "std.h"

namespace Karm::Base {

#define FlagsEnum$(T)                              \
    inline T operator~(T a) {                      \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return (T) ~(U)a;                          \
    }                                              \
    inline T operator|(T a, T b) {                 \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return (T)((U)a | (U)b);                   \
    }                                              \
    inline T operator&(T a, T b) {                 \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return (T)((U)a & (U)b);                   \
    }                                              \
    inline T operator^(T a, T b) {                 \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return (T)((U)a ^ (U)b);                   \
    }                                              \
    inline T &operator|=(T &a, T b) {              \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return (T &)((U &)a |= (U)b);              \
    }                                              \
    inline T &operator&=(T &a, T b) {              \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return (T &)((U &)a &= (U)b);              \
    }                                              \
    inline T &operator^=(T &a, T b) {              \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return (T &)((U &)a ^= (U)b);              \
    }

} // namespace Karm::Base
