module;

#include <karm-core/macros.h>

export module Karm.Core:base.endian;

import :base.slice;

namespace Karm {

export template <typename T>
struct [[gnu::packed]] Be {
    T _value;

    always_inline constexpr Be() = default;

    always_inline constexpr Be(T value)
        : _value(toBe(value)) {}

    always_inline constexpr operator T() const {
        return value();
    }

    always_inline constexpr Bytes bytes() const {
        return Bytes((u8 const*)&_value, sizeof(T));
    }

    always_inline constexpr T value() const {
        return toBe(_value);
    }
};

export template <typename T>
struct [[gnu::packed]] Le {
    T _value;

    always_inline constexpr Le() = default;

    always_inline constexpr Le(T value)
        : _value(toLe(value)) {
    }

    always_inline constexpr operator T() const {
        return value();
    }

    always_inline constexpr Bytes bytes() const {
        return Bytes((u8 const*)&_value, sizeof(T));
    }

    always_inline constexpr T value() const {
        return toLe(_value);
    }
};

static_assert(sizeof(Be<u32>) == sizeof(u32));
static_assert(sizeof(Le<u32>) == sizeof(u32));
static_assert(Meta::Pod<Le<u32>>);
static_assert(Meta::Pod<Be<u32>>);

export using u8be = Be<u8>;
export using u16be = Be<u16>;
export using u32be = Be<u32>;
export using u64be = Be<u64>;

export using i8be = Be<i8>;
export using i16be = Be<i16>;
export using i32be = Be<i32>;
export using i64be = Be<i64>;

export using u8le = Le<u8>;
export using u16le = Le<u16>;
export using u32le = Le<u32>;
export using u64le = Le<u64>;

export using i8le = Le<i8>;
export using i16le = Le<i16>;
export using i32le = Le<i32>;
export using i64le = Le<i64>;

} // namespace Karm
