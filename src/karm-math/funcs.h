#pragma once

#include <karm-meta/traits.h>
#include <tgmath.h>
namespace Karm::Math {

template <Karm::Meta::Float T>
constexpr static inline T floor(T const x) {
    if (x != x) {
        // handle infinities and nan
        return x;
    }
    return (T)(long long)x - 1;
}

template <Karm::Meta::Float T>
constexpr static inline T ceil(T const x) {
    if (x != x) {
        // handle infinities and nan
        return x;
    }
    return (T)(long long)x + 1;
}

} // namespace Karm::Math
