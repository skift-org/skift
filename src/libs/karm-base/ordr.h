#pragma once

#include <karm-meta/traits.h>

#include "std.h"

namespace Karm {

struct Ordr {
    enum Value : i8 {
        LESS = -128,
        EQUAL = 0,
        GREATER = 127,
    };

    Value _value;

    constexpr static Ordr from(isize cmp) {
        return cmp < 0 ? LESS : cmp > 0 ? GREATER
                                        : EQUAL;
    }

    constexpr Ordr(Value value) : _value(value) {}

    constexpr Ordr operator|(Ordr const &other) const {
        if (_value != EQUAL) {
            return other;
        }

        return EQUAL;
    }

    constexpr bool operator==(Ordr const &other) const = default;

    constexpr bool isEq() const { return _value == EQUAL; }
    constexpr bool isNe() const { return _value != EQUAL; }
    constexpr bool isLt() const { return _value == LESS; }
    constexpr bool isGt() const { return _value == GREATER; }
    constexpr bool isLtEq() const { return _value != GREATER; }
    constexpr bool isGtEq() const { return _value != LESS; }
};

template <typename Lhs, typename Rhs = Lhs>
concept Comparable = requires(Lhs const &lhs, Rhs const &rhs) {
                         { lhs.cmp(rhs) } -> Meta::Same<Ordr>;
                     };

template <typename Lhs, typename Rhs = Lhs>
concept ComparaisonOperator = requires(Lhs const &lhs, Rhs const &rhs) {
                                  { lhs < rhs } -> Meta::Same<bool>;
                                  { lhs > rhs } -> Meta::Same<bool>;
                                  { lhs == rhs } -> Meta::Same<bool>;
                              };

template <Comparable T>
constexpr Ordr cmp(T const &lhs, T const &rhs) {
    return lhs.cmp(rhs);
}

// fallback for types that don't implement Comparable
template <typename Lhs, typename Rhs>
    requires(ComparaisonOperator<Lhs, Rhs>)
constexpr Ordr cmp(Lhs const &lhs, Rhs const &rhs) {
    if (lhs < rhs) {
        return Ordr::LESS;
    } else if (lhs > rhs) {
        return Ordr::GREATER;
    } else {
        return Ordr::EQUAL;
    }
}

template <typename T>
Ordr cmp(T const *lhs, usize lhs_len, T const *rhs, usize rhs_len) {
    usize len = lhs_len < rhs_len ? lhs_len : rhs_len;

    for (usize i = 0; i < len; i++) {
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

namespace Op {

template <typename Lhs, typename Rhs>
constexpr bool eq(Lhs const &lhs, Rhs const &rhs) {
    return cmp(lhs, rhs).isEq();
}

template <typename Lhs, typename Rhs>
constexpr bool ne(Lhs const &lhs, Rhs const &rhs) {
    return cmp(lhs, rhs).isNe();
}

template <typename Lhs, typename Rhs>
constexpr bool lt(Lhs const &lhs, Rhs const &rhs) {
    return cmp(lhs, rhs).isLt();
}

template <typename Lhs, typename Rhs>
constexpr bool gt(Lhs const &lhs, Rhs const &rhs) {
    return cmp(lhs, rhs).isGt();
}

template <typename Lhs, typename Rhs>
constexpr bool gteq(Lhs const &lhs, Rhs const &rhs) {
    return cmp(lhs, rhs).isGtEq();
}

template <typename Lhs, typename Rhs>
constexpr bool lteq(Lhs const &lhs, Rhs const &rhs) {
    return cmp(lhs, rhs).isLtEq();
}

} // namespace Op

} // namespace Karm
