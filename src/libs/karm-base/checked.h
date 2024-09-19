#pragma once

#include <karm-meta/traits.h>

#include "limits.h"
#include "res.h"

namespace Karm {
template <Meta::Integral T>
always_inline constexpr bool willAddOverflow(T lhs, T rhs) {
    if (rhs > 0)
        return lhs > (Limits<T>::MAX - rhs);
    return lhs < (Limits<T>::MIN - rhs);
}

template <Meta::Integral T>
always_inline constexpr bool willAddUnderflow(T lhs, T rhs) {
    if (rhs < 0)
        return lhs < (Limits<T>::MIN - rhs);
    return lhs > (Limits<T>::MAX - rhs);
}

template <Meta::Integral T>
always_inline constexpr bool willSubOverflow(T lhs, T rhs) {
    if (rhs < 0)
        return lhs > (Limits<T>::MAX + rhs);
    return lhs < (Limits<T>::MIN + rhs);
}

template <Meta::Integral T>
always_inline constexpr bool willSubUnderflow(T lhs, T rhs) {
    if (rhs > 0)
        return lhs < (Limits<T>::MIN + rhs);
    return lhs > (Limits<T>::MAX + rhs);
}

template <Meta::Integral T>
always_inline constexpr bool willAddUnderAndOverflow(T lhs, T rhs) {
    return willAddUnderflow(lhs, rhs) or willAddOverflow(lhs, rhs);
}

template <Meta::Integral T>
always_inline constexpr bool willSubUnderAndOverflow(T lhs, T rhs) {
    return willSubUnderflow(lhs, rhs) or willSubOverflow(lhs, rhs);
}

template <Meta::Integral T>
always_inline constexpr Res<T> checkedAdd(T lhs, T rhs) {
    if (willAddUnderAndOverflow(lhs, rhs)) [[unlikely]]
        return Error::other("numeric overflow");

    return Ok(lhs + rhs);
}

template <Meta::Integral T>
always_inline constexpr T saturatingAdd(T lhs, T rhs) {
    if (willAddOverflow(lhs, rhs)) [[unlikely]]
        return Limits<T>::MAX;
    if (willAddUnderflow(lhs, rhs)) [[unlikely]]
        return Limits<T>::MIN;
    return lhs + rhs;
}

template <Meta::Integral T>
always_inline constexpr Res<T> checkedSub(T lhs, T rhs) {
    if (willSubOverflow(lhs, rhs)) [[unlikely]]
        return Error::other("numeric overflow");
    if (willSubUnderflow(lhs, rhs)) [[unlikely]]
        return Error::other("numeric underflow");
    return Ok(lhs - rhs);
}

template <Meta::Integral T>
always_inline constexpr T saturatingSub(T lhs, T rhs) {
    if (willSubOverflow(lhs, rhs)) [[unlikely]]
        return Limits<T>::MAX;
    if (willSubUnderflow(lhs, rhs)) [[unlikely]]
        return Limits<T>::MIN;
    return lhs - rhs;
}

template <Meta::Integral T>
always_inline constexpr Res<T> checkedInc(T val) {
    if (willAddOverflow(val, 1)) [[unlikely]]
        return Error::other("numeric overflow");
    return Ok(val + 1);
}

template <Meta::Integral T>
always_inline constexpr T saturatingInc(T val) {
    if (willAddOverflow(val, 1)) [[unlikely]]
        return Limits<T>::MAX;
    return val + 1;
}

template <Meta::Integral T>
always_inline constexpr Res<T> checkedDec(T op) {
    if (op == Limits<T>::MIN) [[unlikely]]
        return Error::other("numeric underflow");
    return Ok(op - 1);
}

template <Meta::Integral T>
always_inline constexpr T saturatingDec(T val) {
    if (val == Limits<T>::MIN) [[unlikely]]
        return Limits<T>::MIN;
    return val - 1;
}

} // namespace Karm
