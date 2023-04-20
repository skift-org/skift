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
struct Be {
    T _value;

    ALWAYS_INLINE constexpr Be() = default;
    ALWAYS_INLINE constexpr Be(T value) : _value(_swapBe(value)) {}
    ALWAYS_INLINE constexpr operator T() const { return _swapBe(_value); }

    ALWAYS_INLINE constexpr Bytes bytes() const { return Bytes((Byte const *)&_value, sizeof(T)); }
};

template <typename T>
struct Le {
    T _value;

    ALWAYS_INLINE constexpr Le() = default;
    ALWAYS_INLINE constexpr Le(T value) : _value(_swapLe(value)) {}
    ALWAYS_INLINE constexpr operator T() const { return _swapLe(_value); }

    ALWAYS_INLINE constexpr Bytes bytes() const { return Bytes((Byte const *)&_value, sizeof(T)); }
};

static_assert(sizeof(Be<u32>) == sizeof(u32));
static_assert(sizeof(Le<u32>) == sizeof(u32));

using u8be = Be<u8>;
using u16be = Be<u16>;
using u32be = Be<u32>;
using u64be = Be<u64>;

using i8be = Be<i8>;
using i16be = Be<i16>;
using i32be = Be<i32>;
using i64be = Be<i64>;

using u8le = Le<u8>;
using u16le = Le<u16>;
using u32le = Le<u32>;
using u64le = Le<u64>;

using i8le = Le<i8>;
using i16le = Le<i16>;
using i32le = Le<i32>;
using i64le = Le<i64>;

} // namespace Karm
