#pragma once

#include <karm-base/clamp.h>

#include "const.h"

namespace Karm::Math {

/* --- Other ---------------------------------------------------------------- */

constexpr auto abs(auto value) {
    return value < 0 ? -value : value;
}

constexpr auto lerp(auto a, auto b, auto p) {
    return a + (b - a) * p;
}

template <typename T>
constexpr bool epsilonEq(T lhs, T rhs, T epsilon) {
    return abs(lhs - rhs) < epsilon;
}

/* --- Floats --------------------------------------------------------------- */

static constexpr bool isNan(f64 x) {
    return __builtin_isnan(x);
}

static constexpr bool isInf(f64 x) {
    return __builtin_isinf(x);
}

static constexpr bool isNegInf(f64 x) {
    return __builtin_isinf_sign(x) < 0;
}

/* --- Rounding + Remainder ------------------------------------------------- */

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

template <typename T, Precision P = PRECISE>
T atan(T x) {
    double result = 0.0;
    double sign = 1.0;
    double y = 1.0;

    if (x < 0) {
        x = -x;
        sign = -1.0;
    }

    while (x > 1.0) {
        x = x - 1.0;
        y = y + 1.0;
    }

    double x2 = x * x;
    double term = x;
    double denom = 1.0;

    for (int i = 1; i <= (P == PRECISE ? 50 : 25); i++) {
        term = term * x2;
        denom = 2 * i + 1;
        double frac = term / denom;
        if (i % 2 == 0) {
            result += frac;
        } else {
            result -= frac;
        }
    }

    return sign * result;
}

template <typename T, Precision P = PRECISE>
T atan2(T y, T x) {
    if (x > 0)
        return atan<T, P>(y / x);

    if (x < 0) {
        if (y >= 0)
            return atan<T, P>(y / x) + PI;

        return atan<T, P>(y / x) - PI;
    }

    if (y > 0)
        return PI / 2;

    if (y < 0)
        return -PI / 2;

    return 0;
}

/* --- Exponentials --------------------------------------------------------- */

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

/* --- Power ---------------------------------------------------------------- */

template <typename T>
constexpr T pow2(T x) {
    return x * x;
}

template <typename T>
auto sqrt(T x) -> T {
    if (x < 0.0)
        return NAN;

    if (x == 0.0 || isNan(x) || isInf(x))
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
