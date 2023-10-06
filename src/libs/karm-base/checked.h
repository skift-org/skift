#pragma once

#include <karm-meta/traits.h>

#include "res.h"

namespace Karm {

template <typename T>
inline constexpr usize BITS = sizeof(T) * CHAR_BIT;

template <typename T>
inline constexpr T MAX = Meta::Signed<T> ? 1 + ~(T(1) << (sizeof(T) * CHAR_BIT - 1)) : ~(T(0));

template <typename T>
inline constexpr T MIN = Meta::Signed<T> ? -(MAX<T> - 1) : 0;

template <typename T>
bool willAddOverflow(T lhs, T rhs) {
    return (lhs > 0 and rhs > 0) and (lhs > MAX<T> - rhs or MAX<T> - lhs < rhs);
}

template <typename T>
bool willAddUnderflow(T lhs, T rhs) {
    return (lhs < 0 and rhs < 0) and (lhs > MIN<T> - rhs or MIN<T> - lhs < rhs);
}

template <typename T>
bool willSubOverflow(T lhs, T rhs) {
    return (lhs > 0 and rhs < 0) and (lhs > MAX<T> - rhs);
}

template <typename T>
bool willSubUnderflow(T lhs, T rhs) {
    return (lhs < 0 and rhs > 0) and (lhs < MIN<T> - rhs);
}

template <typename T>
Res<T> checkedAdd(T lhs, T rhs) {
    if (willAddOverflow(lhs, rhs))
        return Error::other("numeric overflow");
    if (willAddUnderflow(lhs, rhs))
        return Error::other("numeric underflow");
    return Ok(lhs + rhs);
}

template <typename T>
Res<T> checkedSub(T lhs, T rhs) {
    if (willSubOverflow(lhs, rhs))
        return Error::other("numeric overflow");
    if (willSubUnderflow(lhs, rhs))
        return Error::other("numeric underflow");
    return Ok(lhs - rhs);
}

template <typename T>
Res<T> checkedInc(T val) {
    if (willAddOverflow(val, 1))
        return Error::other("numeric overflow");
    return Ok(val + 1);
}

template <typename T>
Res<T> checkedDec(T op) {
    if (op == MIN<T>)
        return Error::other("numeric underflow");
    return Ok(op - 1);
}

} // namespace Karm
