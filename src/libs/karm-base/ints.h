#pragma once

#include <stddef.h>
#include <stdint.h>

namespace Karm {

using usize = size_t;
using isize = ptrdiff_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using u128 = __uint128_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using i128 = __int128_t;

using f16 = __fp16;
using f32 = float;
using f64 = double;
using f128 = long double;

inline constexpr auto rol(auto x, auto n) {
    return (x << n) | (x >> (sizeof(x) * 8 - n));
}

inline constexpr auto ror(auto x, auto n) {
    return (x >> n) | (x << (sizeof(x) * 8 - n));
}

} // namespace Karm
