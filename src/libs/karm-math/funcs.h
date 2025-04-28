#pragma once

#include <karm-base/clamp.h>
#include <karm-base/endian.h>
#include <karm-base/limits.h>

#include "const.h"
#include "float.h"

namespace Karm::Math {

// MARK: Other -----------------------------------------------------------------

template <Meta::Integral T>
constexpr T abs(T value) {
    if (value == Limits<T>::MIN)
        panic("absolute value of MIN is undefined");
    return value < T{} ? -value : value;
}

template <Meta::Float T>
constexpr T abs(T value) {
    return value < T{} ? -value : value;
}

constexpr auto lerp(auto a, auto b, auto p) {
    return a + (b - a) * p;
}

template <Meta::Float T>
constexpr bool epsilonEq(T lhs, T rhs, T epsilon = Limits<T>::EPSILON) {
    return abs(lhs - rhs) < epsilon;
}

// MARK: Floats ----------------------------------------------------------------

constexpr bool isNan(Meta::Float auto x) {
    return __builtin_isnan(x);
}

static constexpr bool isInf(Meta::Float auto x) {
    return __builtin_isinf(x);
}

static constexpr bool isNegInf(Meta::Float auto x) {
    return __builtin_isinf_sign(x) < 0;
}

// MARK: Division --------------------------------------------------------------

template <Meta::Float T>
constexpr T fmod(T x, T y) {
    if constexpr (Meta::Same<T, float>)
        return __builtin_fmodf(x, y);
    if constexpr (Meta::Same<T, double>)
        return __builtin_fmod(x, y);
    if constexpr (Meta::Same<T, long double>)
        return __builtin_fmodl(x, y);
}

// MARK: Rounding + Remainder --------------------------------------------------

template <typename T>
constexpr isize floori(T x) {
    isize inum = (isize)x;
    if ((T)inum == x)
        return inum;
    if (x < T{0})
        return inum - 1;
    return inum;
}

template <typename T>
constexpr isize ceili(T x) {
    isize inum = (isize)x;
    if ((T)inum == x)
        return inum;
    if (x < T{0})
        return inum;
    return inum + 1;
}

template <typename T>
constexpr isize roundi(T x) {
    if (x < T{0})
        return (long)(x - T{0.5});
    return (long)(x + T{0.5});
}

template <typename T>
constexpr T floor(T x) {
    return static_cast<T>(floori(x));
}

template <typename T>
constexpr T ceil(T x) {
    return static_cast<T>(ceili(x));
}

template <typename T>
constexpr T round(T x) {
    return static_cast<T>(roundi(x));
}

// MARK: Trigonometry ----------------------------------------------------------

enum Precision {
    PRECISE,
    FAST,
};

// https://stackoverflow.com/questions/18662261/fastest-implementation-of-sine-cosine-and-square-root-in-c-doesnt-need-to-b/28050328#28050328
template <Meta::Float T, Precision P = PRECISE>
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

template <Meta::Float T, Precision P = PRECISE>
constexpr T sin(T x) noexcept {
    return cos<T, P>(x - T(.5 * PI));
}

template <Meta::Float T, Precision P = PRECISE>
constexpr T tan(T x) noexcept {
    return sin<T, P>(x) / cos<T, P>(x);
}

template <Meta::Float T>
constexpr T atan2(T y, T x) {
    return __builtin_atan2(y, x);
}

template <Meta::Float T>
constexpr T atan(T x) {
    return __builtin_atan(x);
}

template <Meta::Float T, Precision P = PRECISE>
constexpr T cot(T x) noexcept {
    return T(1.) / tan<T, P>(x);
}

// MARK: Exponentials ----------------------------------------------------------

template <Meta::Float T>
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

template <Meta::Integral T>
constexpr T exp2(T exponent) {
    return static_cast<T>(1) << exponent;
}

template <Meta::Float T>
constexpr T exp2(T exponent) {
    return __builtin_exp2(exponent);
}

template <Meta::Float T>
constexpr T log2(T x) {
    // References:
    // Gist comparing some implementations
    // * https://gist.github.com/Hendiadyoin1/f58346d66637deb9156ef360aa158bf9

    if (x == 0)
        return -INF;

    if (x <= 0 or __builtin_isnan(x))
        return NAN;

    FloatBits<T> ext{.d = x};
    T exponent = ext.exponent - FloatBits<T>::exponentBias;

    // When the mantissa shows 0b00 (implicitly 1.0) we are on a power of 2
    if (ext.mantissa == 0)
        return exponent;

    // FIXME: Handle denormalized numbers separately

    // (1 <= mantissa < 2)
    T m;
    if constexpr (isLittleEndian()) {
        m = ((FloatBits<T>){
                 .mantissa = ext.mantissa,
                 .exponent = FloatBits<T>::exponentBias,
                 .sign = ext.sign
             })
                .d;
    } else {
        m = ((FloatBits<T>){
                 .sign = ext.sign,
                 .exponent = FloatBits<T>::exponent_bias,
                 .mantissa = ext.mantissa
             })
                .d;
    }

    // This is a reconstruction of one of Sun's algorithms
    // They use a transformation to lower the problem space,
    // while keeping the precision, and a 14th degree polynomial,
    // which is mirrored at sqrt(2)
    // TODO: Sun has some more algorithms for this and other math functions,
    //       leveraging LUTs, investigate those, if they are better in performance and/or precision.
    //       These seem to be related to crLibM's implementations, for which papers and references
    //       are available.
    // FIXME: Dynamically adjust the amount of precision between floats and doubles
    //        AKA floats might need less accuracy here, in comparison to doubles

    bool inverted = false;
    // m > sqrt(2)
    if (m > SQRT2) {
        inverted = true;
        m = 2 / m;
    }
    T s = (m - 1) / (m + 1);
    // ln((1 + s) / (1 - s)) == ln(m)
    T s2 = s * s;
    // clang-format off
    T high_approx = s2 * (static_cast<T>(0.6666666666666735130)
                  + s2 * (static_cast<T>(0.3999999999940941908)
                  + s2 * (static_cast<T>(0.2857142874366239149)
                  + s2 * (static_cast<T>(0.2222219843214978396)
                  + s2 * (static_cast<T>(0.1818357216161805012)
                  + s2 * (static_cast<T>(0.1531383769920937332)
                  + s2 *  static_cast<T>(0.1479819860511658591)))))));
    // clang-format on

    // ln(m) == 2 * s + s * high_approx
    // log2(m) == log2(e) * ln(m)
    T log2_mantissa = L2_E * (2 * s + s * high_approx);
    if (inverted)
        log2_mantissa = 1 - log2_mantissa;
    return exponent + log2_mantissa;
}

template <typename T>
constexpr T pow2(T x) {
    return x * x;
}

template <Meta::Integral T = isize, Precision = PRECISE>
constexpr T pow(T base, T exponent) {
    if (exponent < 0)
        return 0;
    if (exponent == 0)
        return 1;

    T res = 1;
    while (exponent > 0) {
        if (exponent & 1)
            res *= base;
        base *= base;
        exponent /= 2u;
    }
    return res;
}

template <Meta::Float T = f64, Precision = PRECISE>
constexpr T pow(T base, T exponent) {
    if (isNan(exponent))
        return exponent;
    if (exponent == 0)
        return 1;
    if (base == 0)
        return 0;
    if (exponent == 1)
        return base;
    int y_as_int = (int)exponent;
    if (exponent == (T)y_as_int) {
        T result = base;
        for (int i = 0; i < abs(exponent) - 1; ++i)
            result *= base;
        if (exponent < 0)
            result = 1.0l / result;
        return result;
    }

    return exp2<T>(exponent * log2<T>(base));
}

template <Meta::Float T>
constexpr auto sqrt(T x) -> T {
    return __builtin_sqrt(x);
}

template <Meta::Float T, Precision P = PRECISE>
constexpr T asin(T x) {
    if (x > 1 or x < -1)
        return NAN;

    if (x > (T)0.5 or x < (T)-0.5)
        return 2 * atan<T>(x / (1 + sqrt<T>(1 - x * x)));

    T squared = x * x;
    T value = x;
    T i = x * squared;

    auto odd = [](this auto& self, auto i) {
        if (i == 1)
            return 1;
        return i * self(i - 2);
    };

    auto even = [](this auto& self, auto i) {
        if (i == 0)
            return 2;
        return i * self(i - 2);
    };

    for (auto it = 1; it < 15; it += 2) {
        value += i * odd(it) / even(it + 1);
        i *= squared;
    }

    return value;
}

template <Meta::Float T, Precision P = PRECISE>
constexpr T acos(T value) {
    return static_cast<T>(0.5) * PI - asin<T, P>(value);
}

} // namespace Karm::Math
