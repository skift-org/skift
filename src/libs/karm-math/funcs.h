#pragma once

#include <karm-base/clamp.h>

#include "const.h"

namespace Karm::Math {

constexpr auto abs(auto value) {
    return value < 0 ? -value : value;
}

template <typename T>
constexpr T floor(T x) {
    if (x < 0)
        return (T)(long)(x - 1);

    return (T)(long)x;
}

template <typename T>
constexpr T ceil(T x) {
    if (x < 0)
        return x;

    return (T)(long)(x + 1);
}

constexpr int round(double x) {
    return (int)(x + 0.5);
}

constexpr auto pow2(auto x) {
    return x * x;
}

constexpr auto lerp(auto a, auto b, auto p) {
    return a + (b - a) * p;
}

/*
template <typename T>
constexpr T pow(T x, T n) {
    if (n == 0)
        return 1;

    if (n == 1)
        return x;

    T res = 1;
    while (n > 0) {
        if (n & 1)
            res *= x;
        x *= x;
        n >>= 1;
    }
    return res;
}*/

/* --- Trigonometry --------------------------------------------------------- */

enum Precision {
    PRECISE,
    FAST,
};

// https://stackoverflow.com/questions/18662261/fastest-implementation-of-sine-cosine-and-square-root-in-c-doesnt-need-to-b/28050328#28050328
template <typename T, Precision P = PRECISE>
constexpr T cos(T x) {
    constexpr T tp = 1. / (2. * PI);
    x *= tp;
    x -= T(.25) + floor(x + T(.25));
    x *= T(16.) * (abs(x) - T(.5));
    if constexpr (P == PRECISE) {
        x += T(.225) * x * (abs(x) - T(1.));
    }
    return x;
}

template <typename T, Precision P = PRECISE>
constexpr T sin(T x) noexcept {
    return cos<T, P>(x - T(.5 * PI));
}

template <typename T>
struct SinCos {
    T sin;
    T cos;
};

template <typename T, Precision P = PRECISE>
constexpr SinCos<T> sinCos(T x) noexcept {
    return {sin<T, P>(x), cos<T, P>(x)};
}

template <typename T, Precision P = PRECISE>
constexpr T asin(T x) {
    constexpr T tp = 1. / (2. * PI);
    x *= tp;
    x -= T(.25) + floor(x + T(.25));
    x *= T(16.) * (abs(x) - T(.5));
    if constexpr (P == PRECISE) {
        x += T(.225) * x * (abs(x) - T(1.));
    }
    return T(.5 * PI) - x;
}

template <typename T, Precision P = PRECISE>
constexpr T tan(T x) noexcept {
    return sin<T, P>(x) / cos<T, P>(x);
}

template <typename T, Precision P = PRECISE>
constexpr T cot(T x) noexcept {
    return T(1.) / tan<T, P>(x);
}

/* --- Comparison ----------------------------------------------------------- */

template <typename T>
constexpr bool epsilonEq(T lhs, T rhs, T epsilon) {
    return abs(lhs - rhs) < epsilon;
}

} // namespace Karm::Math
