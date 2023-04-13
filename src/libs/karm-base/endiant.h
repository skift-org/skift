#pragma once

#include "slice.h"
#include "std.h"

namespace Karm {

// clang-format off
// clang-format has trouble with the following code
template <typename T>
requires(sizeof(T) <= 8)
ALWAYS_INLINE T bswap(T value) {
    if (sizeof(T) == 8)
        return __builtin_bswap64(value);
    if (sizeof(T) == 4)
        return __builtin_bswap32(value);
    if (sizeof(T) == 2)
        return __builtin_bswap16(value);
    if (sizeof(T) == 1)
        return value;
}
// clang-format on

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
template <typename T>
ALWAYS_INLINE T _swapLe(T value) {
    return value;
}
#else
template <typename T>
ALWAYS_INLINE T _swapLe(T value) {
    return bswap(value);
}
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
template <typename T>
ALWAYS_INLINE T _swapBe(T value) {
    return bswap(value);
}
#else
template <typename T>
ALWAYS_INLINE T _swapBe(T value) {
    return value;
}
#endif

template <typename T>
struct _be {
    T _value;

    ALWAYS_INLINE constexpr _be() = default;
    ALWAYS_INLINE constexpr _be(T value) : _value(_swapBe(value)) {}
    ALWAYS_INLINE constexpr operator T() const { return _swapBe(_value); }

    ALWAYS_INLINE constexpr Bytes bytes() const { return Bytes((Byte const *)&_value, sizeof(T)); }
};

template <typename T>
struct _le {
    T _value;

    ALWAYS_INLINE constexpr _le() = default;
    ALWAYS_INLINE constexpr _le(T value) : _value(_swapLe(value)) {}
    ALWAYS_INLINE constexpr operator T() const { return _swapLe(_value); }

    ALWAYS_INLINE constexpr Bytes bytes() const { return Bytes((Byte const *)&_value, sizeof(T)); }
};

static_assert(sizeof(_be<u32>) == sizeof(u32));
static_assert(sizeof(_le<u32>) == sizeof(u32));

using u8be = _be<u8>;
using u16be = _be<u16>;
using u32be = _be<u32>;
using u64be = _be<u64>;

using i8be = _be<i8>;
using i16be = _be<i16>;
using i32be = _be<i32>;
using i64be = _be<i64>;

using u8le = _le<u8>;
using u16le = _le<u16>;
using u32le = _le<u32>;
using u64le = _le<u64>;

using i8le = _le<i8>;
using i16le = _le<i16>;
using i32le = _le<i32>;
using i64le = _le<i64>;

} // namespace Karm
