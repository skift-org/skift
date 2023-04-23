#pragma once

#include <karm-meta/traits.h>

#include "std.h"

namespace Karm {

template <typename T>
struct Flags {
    T _value = {};

    Flags() = default;

    Flags(T value)
        : _value(value) {}

    bool has(T value) const {
        return (_value & value) == value;
    }

    void set(T value) {
        _value |= value;
    }

    void unset(T value) {
        _value &= ~value;
    }

    void toggle(T value) {
        _value ^= value;
    }

    void clear() {
        _value = {};
    }

    T value() const {
        return _value;
    }

    operator T() const {
        return _value;
    }
};

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
    inline bool operator!(T a) {                   \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return not(U) a;                           \
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

} // namespace Karm
