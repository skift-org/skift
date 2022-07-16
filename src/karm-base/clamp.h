#pragma once

#include "_prelude.h"

namespace Karm {

constexpr auto max(auto value) { return value; }
constexpr auto max(auto first, auto... rest) {
    auto rhs = max(rest...);
    return first > rhs ? first : rhs;
}

constexpr auto min(auto value) { return value; }
constexpr auto min(auto first, auto... rest) {
    auto rhs = min(rest...);
    return first < rhs ? first : rhs;
}

constexpr auto clamp(auto value, auto min, auto max) {
    return value < min ? min : value > max ? max
                                           : value;
}

constexpr auto clamp01(auto value) {
    return value < 0 ? 0 : value > 1 ? 1
                                     : value;
}

} // namespace Karm
