#pragma once

#include <karm-base/clamp.h>
#include <karm-base/limits.h>

#include "const.h"

namespace Karm::Math {

// MARK: Other -----------------------------------------------------------------

template <typename T>
constexpr auto abs(T value) {
    return value < T{} ? -value : value;
}

constexpr auto lerp(auto a, auto b, auto p) {
    return a + (b - a) * p;
}

template <typename T>
constexpr bool epsilonEq(T lhs, T rhs, T epsilon = Limits<T>::EPSILON) {
    return abs(lhs - rhs) < epsilon;
}

// MARK: Floats ----------------------------------------------------------------

static constexpr bool isNan(f64 x) {
    return __builtin_isnan(x);
}

static constexpr bool isInf(f64 x) {
    return __builtin_isinf(x);
}

static constexpr bool isNegInf(f64 x) {
    return __builtin_isinf_sign(x) < 0;
}

// MARK: Rounding + Remainder --------------------------------------------------

template <typename T>
constexpr isize floori(T x) {
    if (x < 0)
        return (long)(x - 1);

    return (long)x;
}

template <typename T>
constexpr isize ceili(T x) {
    if (x < 0)
        return (long)x;

    return (long)(x + 1);
}

template <typename T>
constexpr isize roundi(T x) {
    return (long)(x + 0.5);
}

template <typename T>
constexpr T floor(T x) {
    if (x < T{})
        return (T)(long)(x - T{1});

    return (T)(long)x;
}

template <typename T>
constexpr T ceil(T x) {
    if (x < T{})
        return (T)(long)x;

    return (T)(long)(x + T{1});
}

template <typename T>
constexpr T round(T x) {
    return (T)(long)(x + T{0.5});
}

// MARK: Trigonometry ----------------------------------------------------------

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
    return {
        sin<T, P>(x),
        cos<T, P>(x),
    };
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

// MARK: Exponentials ----------------------------------------------------------

template <typename T>
T exp(T x) {
    if (x == 0.0)
        return 1;

    if (isNegInf(x))
        return 0;

    if (isInf(x) == 1)
        return x;

    if (isNan(x))
        return x;

    if (x < 0.0)
        return (1.0 / exp(-x));

    T counter = 0;
    T result = 1.0;
    T term = 1.0;

    while (++counter, x / counter > (x / (x * x + 42.0))) {
        term *= x / counter;
        result += term;
    }

    return (result);
}

// MARK: Power -----------------------------------------------------------------

template <typename T>
constexpr T pow2(T x) {
    return x * x;
}

template <Meta::Integral T = isize, Precision = PRECISE>
constexpr T pow(T a, T b) {
    T result = 1;
    while (b > 0) {
        if (b & 1)
            result *= a;
        a *= a;
        b >>= 1;
    }
    return result;
}

template <Meta::Float T = f64, Precision = PRECISE>
constexpr T pow(T a, T b) {
    // FIXME: Don't use std::pow
    return ::pow(a, b);
}

template <typename T>
auto sqrt(T x) -> T {
    if (x < 0.0)
        return NAN;

    if (x == 0.0 or isNan(x) or isInf(x))
        return x;

    auto guess = x / 2;
    auto last = guess + 1;
    while (guess != last) {
        last = guess;
        guess = (guess + x / guess) / 2;
    }
    return guess;
}

} // namespace Karm::Math
