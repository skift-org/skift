#pragma once

#include <karm-meta/traits.h>

#include "limits.h"

namespace Karm {

/// Returns the maximum of a list of values.
always_inline constexpr auto max(auto value) { return value; }

/// Returns the maximum of a list of values.
always_inline constexpr auto max(auto first, auto... rest) {
    auto rhs = max(rest...);
    return first > rhs ? first : rhs;
}

/// Returns the minimum of a list of values.
always_inline constexpr auto min(auto value) { return value; }

/// Returns the minimum of a list of values.
always_inline constexpr auto min(auto first, auto... rest) {
    auto rhs = min(rest...);
    return first < rhs ? first : rhs;
}

/// Clamps a value to a range.
/// If the value is less than the minimum, the minimum is returned.
/// If the value is greater than the maximum, the maximum is returned.
/// Otherwise, the value is returned.
always_inline constexpr auto clamp(auto value, auto min, auto max) {
    return value < min
               ? min
           : value > max
               ? max
               : value;
}

/// Clamps a value to the range [0, 1].
always_inline constexpr auto clamp01(auto value) {
    return clamp(value, 0, 1);
}

#ifndef __ck_freestanding__

/// Rounds a floating point value to the nearest integral value.
template <Meta::Integral I, Meta::Float P>
always_inline constexpr I roundTo(P value) {
    if constexpr (Meta::Same<P, long double>)
        return static_cast<I>(__builtin_llrintl(value));
    if constexpr (Meta::Same<P, double>)
        return static_cast<I>(__builtin_llrint(value));
    if constexpr (Meta::Same<P, float>)
        return static_cast<I>(__builtin_llrintf(value));
}

#endif // !__ck_freestanding__

/// Clamps a value to the range of a given type.
template <typename T>
always_inline constexpr T clampTo(auto val) {
    if (val >= Limits<T>::MAX)
        return Limits<T>::MAX;
    if (val <= Limits<T>::MIN)
        return Limits<T>::MIN;

    if constexpr (Meta::Float<T>)
        return roundTo<T>(val);
    else
        return static_cast<T>(val);
}

} // namespace Karm
