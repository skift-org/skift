#pragma once

#include "macros.h"
#include "ordr.h"
#include "std.h"

namespace Karm {

ALWAYS_INLINE constexpr auto max(auto value) { return value; }

ALWAYS_INLINE constexpr auto max(auto first, auto... rest) {
    auto rhs = max(rest...);
    return Op::gt(first, rhs) ? first : rhs;
}

ALWAYS_INLINE constexpr auto min(auto value) { return value; }
ALWAYS_INLINE constexpr auto min(auto first, auto... rest) {
    auto rhs = min(rest...);
    return Op::lt(first, rhs) ? first : rhs;
}

ALWAYS_INLINE constexpr auto clamp(auto value, auto min, auto max) {
    return Op::lt(value, min)
               ? min
           : Op::gt(value, max)
               ? max
               : value;
}

ALWAYS_INLINE constexpr auto clamp01(auto value) {
    return clamp(value, 0, 1);
}

} // namespace Karm
