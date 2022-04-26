#pragma once

#include <karm-meta/same.h>

#include "_prelude.h"

#include "clamp.h"

namespace Karm::Base {

struct Ordr {
    enum Value {
        LESS = -1,
        EQUAL = 0,
        GREATER = 1,
    };

    Value _value;

    constexpr static Ordr from(int cmp) {
        return (Value)clamp(cmp, LESS, GREATER);
    }

    constexpr Ordr(Value value) : _value(value) {}

    bool operator==(const Ordr &other) const = default;

    constexpr bool is_eq() const { return _value == EQUAL; }
    constexpr bool is_ne() const { return _value != EQUAL; }
    constexpr bool is_lt() const { return _value == LESS; }
    constexpr bool is_gt() const { return _value == GREATER; }
    constexpr bool is_lteq() const { return _value != GREATER; }
    constexpr bool is_gteq() const { return _value != LESS; }
};

namespace Op {

constexpr bool eq(auto const &lhs, auto const &rhs) {
    return lhs.cmp(rhs).is_eq();
}

constexpr bool ne(auto const &lhs, auto const &rhs) {
    return lhs.cmp(rhs).is_ne();
}

constexpr bool lt(auto const &lhs, auto const &rhs) {
    return lhs.cmp(rhs).is_lt();
}

constexpr bool gt(auto const &lhs, auto const &rhs) {
    return lhs.cmp(rhs).is_lt();
}

constexpr bool gteq(auto const &lhs, auto const &rhs) {
    return lhs.cmp(rhs).is_gteq();
}

constexpr bool lteq(auto const &lhs, auto const &rhs) {
    return lhs.cmp(rhs).is_lteq();
}

} // namespace Op

template <typename T>
concept Ordered = requires(T const &lhs, T const &rhs) {
    { lhs.cmp(rhs) } -> Meta::Same<Ordr>;
};

} // namespace Karm::Base
