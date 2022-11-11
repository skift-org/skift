#pragma once

#include "_prelude.h"

#include "ordr.h"

namespace Karm {

constexpr auto max(auto value) { return value; }

constexpr auto max(auto first, auto... rest) {
    auto rhs = max(rest...);
    return Op::gt(first, rhs) ? first : rhs;
}

constexpr auto min(auto value) { return value; }
constexpr auto min(auto first, auto... rest) {
    auto rhs = min(rest...);
    return Op::lt(first, rhs) ? first : rhs;
}

constexpr auto clamp(auto value, auto min, auto max) {
    return Op::lt(value, min)
               ? min
           : Op::gt(value, max)
               ? max
               : value;
}

constexpr auto clamp01(auto value) {
    return clamp(value, 0, 1);
}

} // namespace Karm
