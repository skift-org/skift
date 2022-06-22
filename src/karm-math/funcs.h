#pragma once

namespace Karm::Math {

template <typename T>
constexpr static inline T floor(T const x) {
    return (T)(int)x;
}

template <typename T>
constexpr static inline T ceil(T const x) {
    return (T)(int)x + 1;
}

} // namespace Karm::Math
