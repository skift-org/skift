#pragma once

#include <karm-base/clamp.h>

namespace Karm::Math {

constexpr auto abs(auto value) {
    return value < 0 ? -value : value;
}

static inline double floor(double const x) {
    return (double)(int)x;
}

static inline double ceil(double const x) {
    return (double)(int)x + 1;
}

static inline auto pow2(auto x) {
    return x * x;
}

constexpr bool epsilonEq(double const &lhs, double const &rhs, double epsilon) {
    return abs(lhs - rhs) < epsilon;
}

} // namespace Karm::Math
