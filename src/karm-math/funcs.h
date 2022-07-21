#pragma once

namespace Karm::Math {

static inline double floor(double const x) {
    return (double)(int)x;
}

static inline double ceil(double const x) {
    return (double)(int)x + 1;
}

static inline auto pow2(auto x) {
    return x * x;
}

} // namespace Karm::Math
