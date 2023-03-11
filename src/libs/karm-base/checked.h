#pragma once

#include <karm-meta/traits.h>

#include "res.h"

namespace Karm {

template <typename T>
inline constexpr T MAX = Meta::Signed<T> ? 1 + ~(T(1) << (sizeof(T) * CHAR_BIT - 1)) : ~(T(0));

template <typename T>
inline constexpr T MIN = Meta::Signed<T> ? -(MAX<T> - 1) : 0;

template <typename T>
Res<T> checkedAdd(T op1, T op2) {
    if (op1 > 0 and op2 > 0 and op1 > MAX<T> - op2) {
        return Error::other("numeric overflow");
    }
    if (op1 < 0 and op2 < 0 and op1 < MIN<T> - op2) {
        return Error::other("numeric underflow");
    }
    return {op1 + op2};
}

template <typename T>
Res<T> checkedSub(T op1, T op2) {
    if (op1 > 0 and op2 < 0 and op1 > MAX<T> + op2) {
        return Error::other("numeric overflow");
    }
    if (op1 < 0 and op2 > 0 and op1 < MIN<T> + op2) {
        return Error::other("numeric underflow");
    }
    return {op1 - op2};
}

template <typename T>
Res<T> checkedInc(T op) {
    if (op == MAX<T>) {
        return Error::other("numeric overflow");
    }
    return {op + 1};
}

template <typename T>
Res<T> checkedDec(T op) {
    if (op == MIN<T>) {
        return Error::other("numeric underflow");
    }
    return {op - 1};
}

} // namespace Karm
