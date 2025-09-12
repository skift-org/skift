module;

#include <karm-core/macros.h>

export module Karm.Core:base.base;

import :base.std;

namespace Karm {

// MARK: Unsigned --------------------------------------------------------------

export using usize = __SIZE_TYPE__;

export using u8 = std::uint8_t;
export using u16 = std::uint16_t;
export using u32 = std::uint32_t;
export using u64 = std::uint64_t;
#ifdef __SIZEOF_INT128__
export using u128 = __uint128_t;

inline u128 _bswap128(u128 value) {
    u64 high = __builtin_bswap64(static_cast<u64>(value));
    u64 low = __builtin_bswap64(static_cast<u64>(value >> 64));
    return (static_cast<u128>(high) << 64) | low;
}
#endif

export template <typename T>
    requires(sizeof(T) <= 16)
always_inline constexpr T bswap(T value) {
#ifdef __SIZEOF_INT128__
    if (sizeof(T) == 16)
        return _bswap128(value);
#endif
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
export template <typename T>
always_inline constexpr T toLe(T value) {
    return value;
}
#else
export template <typename T>
always_inline constexpr T toLe(T value) {
    return bswap(value);
}
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
export template <typename T>
always_inline constexpr T toBe(T value) {
    return bswap(value);
}
#else
export template <typename T>
always_inline constexpr T toBe(T value) {
    return value;
}
#endif

export constexpr bool isLittleEndian() {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return true;
#else
    return false;
#endif
}

export constexpr bool isBigEndian() {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return true;
#else
    return false;
#endif
}

export template <typename T>
always_inline constexpr usize popcount(T value) {
    usize count = 0;
    for (usize i = 0; i < sizeof(T) * 8; i++)
        if (value & (1uz << i))
            count++;
    return count;
}

export template <typename T>
always_inline constexpr T rol(T x, usize n) {
    return (x << n) | (x >> (sizeof(x) * 8 - n));
}

export template <typename T>
always_inline constexpr T ror(T x, usize n) {
    return (x >> n) | (x << (sizeof(x) * 8 - n));
}

export template <typename T>
always_inline constexpr T rotl(T x, usize n) {
    return (x << n) | (x >> (sizeof(x) * 8 - n));
}

export template <typename T>
always_inline constexpr T rotr(T x, usize n) {
    return (x >> n) | (x << (sizeof(x) * 8 - n));
}

// MARK: Signed -----------------------------------------------------------

export using isize = __PTRDIFF_TYPE__;

export using i8 = std::int8_t;
export using i16 = std::int16_t;
export using i32 = std::int32_t;
export using i64 = std::int64_t;

#ifdef __SIZEOF_INT128__
export using i128 = __int128_t;
#endif

// MARK: Floating point --------------------------------------------------------

export using f32 = float;
export using f64 = double;

#ifdef __SIZEOF_FLOAT128__
export using f128 = long double;
#endif

// MARK: Tags ------------------------------------------------------------------

/* The object should take the ownership of the memory */

export struct Move {};

export auto MOVE = Move{};

/* The object should make a copy of the memory */

export struct Copy {};

export auto COPY = Copy{};

/* The object should wrap the memory without doing a copy
   nor taking the ownership */

export struct Wrap {};

export auto WRAP = Wrap{};

/* The object should be empty initialized */

export struct None {
    constexpr None() {}

    explicit operator bool() const { return false; }

    bool operator==(None const&) const = default;
    auto operator<=>(None const&) const = default;
};

export constexpr auto NONE = None{};

export template <typename T>
bool operator==(None, T* ptr) {
    return ptr == nullptr;
}

// MARK: Misc ------------------------------------------------------------------

/// A linker symbol.
export using ExternSym = std::uint8_t[];

// MARK: Utilities -------------------------------------------------------------

export template <typename T, typename U>
always_inline T unionCast(U value)
    requires(sizeof(T) == sizeof(U))
{
    union X {
        U u;
        T t;
    };

    return X{.u = value}.t;
}

} // namespace Karm
