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

constexpr isize round(f64 x) {
    return (isize)(x + 0.5);
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

constexpr double atan(double x) {
    double y;

    if (isnan(x))
        return (x);
    if (x == 0.0)
        return (0.0);
    else if (x > 0.0) {
        if (x > 1.0)
            return (M_PI / 2 - atan(1.0 / x));
        else {
            y = (x * x);
            return (x * (1.0 -
                         y * (0.33333333333333333 - y * (0.20000000000000000 -
                                                         y * (0.14285714285714285 - y * (0.11111111111111111 -
                                                                                         y * (0.09090909090909091 - y * (0.07692307692307693))))))));
        }
    } else
        return (-atan(-x));
}

constexpr double atan2(double y, double x) {
    if (isnan(x) || isnan(y))
        return (0.0 / 0.0);
    if (x == 0) {
        if (y > 0)
            return (M_PI_2);
        if (y < 0)
            return (-M_PI_2);
        return (0);
    }
    if (!(isinf(y) && isinf(x)))
        return (atan(y / x));
    if (x > 0) {
        if (y > 0)
            return (M_PI_4);
        return (-M_PI_4);
    }
    if (y > 0)
        return (3 * M_PI_4);
    return (-3 * M_PI_4);
}

/* --- Comparison ----------------------------------------------------------- */

template <typename T>
constexpr bool epsilonEq(T lhs, T rhs, T epsilon) {
    return abs(lhs - rhs) < epsilon;
}

} // namespace Karm::Math
