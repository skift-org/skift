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

    constexpr auto operator==(const Ordr &other) const -> bool = default;

    constexpr auto is_eq() const -> bool { return _value == EQUAL; }
    constexpr auto is_ne() const -> bool { return _value != EQUAL; }
    constexpr auto is_lt() const -> bool { return _value == LESS; }
    constexpr auto is_gt() const -> bool { return _value == GREATER; }
    constexpr auto is_lteq() const -> bool { return _value != GREATER; }
    constexpr auto is_gteq() const -> bool { return _value != LESS; }
};

namespace Op {

constexpr auto eq(auto const &lhs, auto const &rhs) -> bool {
    return lhs.cmp(rhs).is_eq();
}

constexpr auto ne(auto const &lhs, auto const &rhs) -> bool {
    return lhs.cmp(rhs).is_ne();
}

constexpr auto lt(auto const &lhs, auto const &rhs) -> bool {
    return lhs.cmp(rhs).is_lt();
}

constexpr auto gt(auto const &lhs, auto const &rhs) -> bool {
    return lhs.cmp(rhs).is_lt();
}

constexpr auto gteq(auto const &lhs, auto const &rhs) -> bool {
    return lhs.cmp(rhs).is_gteq();
}

constexpr auto lteq(auto const &lhs, auto const &rhs) -> bool {
    return lhs.cmp(rhs).is_lteq();
}

} // namespace Op

template <typename T>
concept Ordered = requires(T const &lhs, T const &rhs) {
    { lhs.cmp(rhs) } -> Meta::Same<Ordr>;
};

} // namespace Karm::Base
