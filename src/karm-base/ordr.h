#pragma once

#include <karm-meta/traits.h>

#include "_prelude.h"

#include "clamp.h"

namespace Karm {

struct Ordr {
    enum Value : int8_t {
        LESS = -128,
        EQUAL = 0,
        GREATER = 127,
    };

    Value _value;

    constexpr static Ordr from(int cmp) {
        return (Value)clamp(cmp, LESS, GREATER);
    }

    constexpr Ordr(Value value) : _value(value) {}

    constexpr Ordr operator|(const Ordr &other) const {
        if (_value != EQUAL) {
            return other;
        }

        return EQUAL;
    }

    constexpr bool operator==(const Ordr &other) const = default;

    constexpr bool isEq() const { return _value == EQUAL; }
    constexpr bool isNe() const { return _value != EQUAL; }
    constexpr bool isLt() const { return _value == LESS; }
    constexpr bool isGt() const { return _value == GREATER; }
    constexpr bool isLtEq() const { return _value != GREATER; }
    constexpr bool isGtEq() const { return _value != LESS; }
};

template <typename T>
concept Comparable = requires(T const &lhs, T const &rhs) {
    { lhs.cmp(rhs) } -> Meta::Same<Ordr>;
};

namespace Op {

template <Comparable T>
constexpr Ordr cmp(T const &lhs, T const &rhs) {
    return lhs.cmp(rhs);
}

// fallback for types that don't implement Comparable
template <typename T>
requires(!Comparable<T>) constexpr Ordr cmp(T const &lhs, T const &rhs) {
    if (lhs < rhs) {
        return Ordr::LESS;
    } else if (lhs > rhs) {
        return Ordr::GREATER;
    } else {
        return Ordr::EQUAL;
    }
}

template <typename T>
Ordr cmp(T const *lhs, size_t lhs_len, T const *rhs, size_t rhs_len) {
    size_t len = min(lhs_len, rhs_len);

    for (size_t i = 0; i < len; i++) {
        Ordr c = cmp(lhs[i], rhs[i]);
        if (c != Ordr::EQUAL) {
            return c;
        }
    }

    if (lhs_len < rhs_len) {
        return Ordr::LESS;
    } else if (lhs_len > rhs_len) {
        return Ordr::GREATER;
    } else {
        return Ordr::EQUAL;
    }
}

template <typename T>
constexpr bool eq(T const &lhs, T const &rhs) {
    return cmp(lhs, rhs).isEq();
}

template <typename T>
constexpr bool ne(T const &lhs, T const &rhs) {
    return cmp(lhs, rhs).isNe();
}

template <typename T>
constexpr bool lt(T const &lhs, T const &rhs) {
    return cmp(lhs, rhs).isLt();
}

template <typename T>
constexpr bool gt(T const &lhs, T const &rhs) {
    return cmp(lhs, rhs).isGt();
}

template <typename T>
constexpr bool gteq(T const &lhs, T const &rhs) {
    return cmp(lhs, rhs).isGtEq();
}

template <typename T>
constexpr bool lteq(T const &lhs, T const &rhs) {
    return cmp(lhs, rhs).isLtEq();
}

} // namespace Op

} // namespace Karm
