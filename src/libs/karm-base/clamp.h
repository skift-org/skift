#pragma once

#include "macros.h"
#include "std.h"

namespace Karm {

always_inline constexpr auto max(auto value) { return value; }

always_inline constexpr auto max(auto first, auto... rest) {
    auto rhs = max(rest...);
    return first > rhs ? first : rhs;
}

always_inline constexpr auto min(auto value) { return value; }
always_inline constexpr auto min(auto first, auto... rest) {
    auto rhs = min(rest...);
    return first < rhs ? first : rhs;
}

always_inline constexpr auto clamp(auto value, auto min, auto max) {
    return value < min
               ? min
           : value > max
               ? max
               : value;
}

always_inline constexpr auto clamp01(auto value) {
    return clamp(value, 0, 1);
}

} // namespace Karm
