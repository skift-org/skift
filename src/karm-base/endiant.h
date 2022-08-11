#pragma once

#include "std.h"

namespace Karm {

// clang-format off
// clang-format has trouble with the following code
template <typename T>
requires(sizeof(T) <= 8)
T bswap(T value) {
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
T _swapLe(T value) {
    return value;
}
#else
template <typename T>
T _swapLe(T value) {
    return bswap(value);
}
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
template <typename T>
T _swapBe(T value) {
    return bswap(value);
}
#else
template <typename T>
T _swapBe(T value) {
    return value;
}
#endif

template <typename T>
struct [[gnu::packed]] _Be {
    T _value;

    constexpr _Be() = default;
    constexpr _Be(T value) : _value(_swapBe(value)) {}
    constexpr operator T() const { return _swapBe(_value); }
};

template <typename T>
struct [[gnu::packed]] _Le {
    T _value;

    constexpr _Le() = default;
    constexpr _Le(T value) : _value(_swapLe(value)) {}
    constexpr operator T() const { return _swapLe(_value); }
};

using be_uint8_t = _Be<uint8_t>;
using be_uint16_t = _Be<uint16_t>;
using be_uint32_t = _Be<uint32_t>;
using be_uint64_t = _Be<uint64_t>;

using be_int8_t = _Be<int8_t>;
using be_int16_t = _Be<int16_t>;
using be_int32_t = _Be<int32_t>;
using be_int64_t = _Be<int64_t>;

using le_uint8_t = _Le<uint8_t>;
using le_uint16_t = _Le<uint16_t>;
using le_uint32_t = _Le<uint32_t>;
using le_uint64_t = _Le<uint64_t>;

using le_int8_t = _Le<int8_t>;
using le_int16_t = _Le<int16_t>;
using le_int32_t = _Le<int32_t>;
using le_int64_t = _Le<int64_t>;

} // namespace Karm
