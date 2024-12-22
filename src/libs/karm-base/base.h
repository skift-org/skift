#pragma once

#include <karm-meta/traits.h>

#include "_prelude.h"
#include "macros.h"

namespace Karm {

// MARK: Unsigned --------------------------------------------------------------

using usize = __SIZE_TYPE__;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
#ifdef __SIZEOF_INT128__
using u128 = __uint128_t;
#endif

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

template <typename T>
always_inline constexpr T rol(T x, usize n) {
    return (x << n) | (x >> (sizeof(x) * 8 - n));
}

template <typename T>
always_inline constexpr T ror(T x, usize n) {
    return (x >> n) | (x << (sizeof(x) * 8 - n));
}

template <typename T>
always_inline constexpr T rotl(T x, usize n) {
    return (x << n) | (x >> (sizeof(x) * 8 - n));
}

template <typename T>
always_inline constexpr T rotr(T x, usize n) {
    return (x >> n) | (x << (sizeof(x) * 8 - n));
}

// MARK: Signed -----------------------------------------------------------

using isize = __PTRDIFF_TYPE__;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

#ifdef __SIZEOF_INT128__
using i128 = __int128_t;
#endif

// MARK: Floating point --------------------------------------------------------

using f16 = __fp16;
using f32 = float;
using f64 = double;
using f128 = long double;

// MARK: Boolean type ----------------------------------------------------------

struct Bool {
    bool _val;

    Bool() = delete;

    constexpr Bool(Meta::Boolean auto value)
        : _val(value) {}

    constexpr Bool &operator=(Meta::Boolean auto value) {
        _val = value;
        return *this;
    }

    explicit constexpr operator bool() const {
        return _val;
    }

    constexpr Bool operator!() const {
        return Bool(not _val);
    }
};

constexpr bool operator==(Bool a, Bool b) {
    return static_cast<bool>(a) == static_cast<bool>(b);
}

constexpr bool operator==(Bool a, Meta::Boolean auto b) {
    return static_cast<bool>(a) == static_cast<bool>(b);
}

constexpr bool operator==(Meta::Boolean auto a, Bool &b) {
    return static_cast<bool>(a) == static_cast<bool>(b);
}

constexpr bool operator!=(Bool a, Bool b) {
    return static_cast<bool>(a) != static_cast<bool>(b);
}

constexpr bool operator!=(Bool a, Meta::Boolean auto b) {
    return static_cast<bool>(a) != static_cast<bool>(b);
}

constexpr bool operator!=(Meta::Boolean auto a, Bool b) {
    return static_cast<bool>(a) != static_cast<bool>(b);
}

// MARK: Tags ------------------------------------------------------------------

/* The object should take the ownership of the memory */

struct Move {};

constexpr inline auto MOVE = Move{};

/* The object should make a copy of the memory */

struct Copy {};

constexpr inline auto COPY = Copy{};

/* The object should wrap the memory without doing a copy
   nor taking the ownership */

struct Wrap {};

constexpr inline auto WRAP = Wrap{};

/* The object should be empty initialized */

struct None {
    constexpr None() {}

    explicit operator bool() const { return false; }

    bool operator==(None const &) const = default;
    auto operator<=>(None const &) const = default;
};

constexpr inline auto NONE = None{};

template <typename T>
bool operator==(None, T *ptr) {
    return ptr == nullptr;
}

// MARK: Misc ------------------------------------------------------------------

/// A linker symbol.
using ExternSym = uint8_t[];

// MARK: Utilities -------------------------------------------------------------

template <typename T, typename U>
always_inline static inline T unionCast(U value)
    requires(sizeof(T) == sizeof(U))
{
    union X {
        U u;
        T t;
    };

    return X{.u = value}.t;
}

} // namespace Karm
