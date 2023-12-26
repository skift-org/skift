#pragma once

#include "slice.h"
#include "std.h"

namespace Karm {

template <typename T>
struct Be {
    T _value;

    always_inline constexpr Be() = default;
    always_inline constexpr Be(T value) : _value(toBe(value)) {}
    always_inline constexpr operator T() const { return toBe(_value); }
    always_inline constexpr Bytes bytes() const { return Bytes((Byte const *)&_value, sizeof(T)); }
};

template <typename T>
struct Le {
    T _value;

    always_inline constexpr Le() = default;
    always_inline constexpr Le(T value) : _value(toLe(value)) {}
    always_inline constexpr operator T() const { return toLe(_value); }
    always_inline constexpr Bytes bytes() const { return Bytes((Byte const *)&_value, sizeof(T)); }
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
