#pragma once

#include <stddef.h>
#include <stdint.h>

#include "macros.h"

namespace Karm {

/* --- Unsigned -------------------------------------------------------- */

using usize = size_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using u128 = __uint128_t;

template <typename T>
    requires(sizeof(T) <= 8)
always_inline constexpr T bswap(T value) {
    if (sizeof(T) == 8)
        return __builtin_bswap64(value);
    if (sizeof(T) == 4)
        return __builtin_bswap32(value);
    if (sizeof(T) == 2)
        return __builtin_bswap16(value);
    if (sizeof(T) == 1)
        return value;
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
template <typename T>
always_inline constexpr T toLe(T value) {
    return value;
}
#else
template <typename T>
always_inline constexpr T toLe(T value) {
    return bswap(value);
}
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
template <typename T>
always_inline constexpr T toBe(T value) {
    return bswap(value);
}
#else
template <typename T>
always_inline constexpr T toBe(T value) {
    return value;
}
#endif

template <typename T>
always_inline constexpr usize popcount(T value) {
    usize count = 0;
    for (usize i = 0; i < sizeof(T) * 8; i++)
        if (value & (1uz << i))
            count++;
    return count;
}

always_inline constexpr auto rol(auto x, auto n) {
    return (x << n) | (x >> (sizeof(x) * 8 - n));
}

always_inline constexpr auto ror(auto x, auto n) {
    return (x >> n) | (x << (sizeof(x) * 8 - n));
}

always_inline constexpr auto rotl(auto x, auto n) {
    return (x << n) | (x >> (sizeof(x) * 8 - n));
}

always_inline constexpr auto rotr(auto x, auto n) {
    return (x >> n) | (x << (sizeof(x) * 8 - n));
}

/* --- Signed ---------------------------------------------------------- */

using isize = ptrdiff_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using i128 = __int128_t;

/* --- Floating point -------------------------------------------------- */

using f16 = __fp16;
using f32 = float;
using f64 = double;
using f128 = long double;

} // namespace Karm
