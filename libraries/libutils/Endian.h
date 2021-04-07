#pragma once

#include <libsystem/Common.h>

template <typename TValue>
constexpr TValue swap_little_endian(TValue value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return value;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    if constexpr (sizeof(TValue) == 8)
        return __builtin_bswap64(value);
    if constexpr (sizeof(TValue) == 4)
        return __builtin_bswap32(value);
    if constexpr (sizeof(TValue) == 2)
        return __builtin_bswap16(value);
    if constexpr (sizeof(TValue) == 1)
        return value;
#endif
}

template <typename TValue>
constexpr TValue swap_big_endian(TValue value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    if constexpr (sizeof(TValue) == 8)
        return __builtin_bswap64(value);
    if constexpr (sizeof(TValue) == 4)
        return __builtin_bswap32(value);
    if constexpr (sizeof(TValue) == 2)
        return __builtin_bswap16(value);
    if constexpr (sizeof(TValue) == 1)
        return value;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return value;
#endif
}

template <typename TValue>
class PACKED BigEndian
{
private:
    TValue _value{0};

public:
    constexpr BigEndian() {}

    constexpr BigEndian(TValue value) : _value(swap_big_endian(value)) {}

    constexpr TValue operator()() const
    {
        return swap_big_endian(_value);
    }
};

template <typename TValue>
class PACKED LittleEndian
{
private:
    TValue _value{0};

public:
    constexpr LittleEndian() {}

    constexpr LittleEndian(TValue value) : _value(swap_little_endian(value)) {}

    constexpr TValue operator()() const
    {
        return swap_little_endian(_value);
    }
};

using be_uint8_t = BigEndian<uint8_t>;
using be_uint16_t = BigEndian<uint16_t>;
using be_uint32_t = BigEndian<uint32_t>;
using be_uint64_t = BigEndian<uint64_t>;

using le_uint8_t = LittleEndian<uint8_t>;
using le_uint16_t = LittleEndian<uint16_t>;
using le_uint32_t = LittleEndian<uint32_t>;
using le_uint64_t = LittleEndian<uint64_t>;

using be_int8_t = BigEndian<int8_t>;
using be_int16_t = BigEndian<int16_t>;
using be_int32_t = BigEndian<int32_t>;
using be_int64_t = BigEndian<int64_t>;

using le_int8_t = LittleEndian<int8_t>;
using le_int16_t = LittleEndian<int16_t>;
using le_int32_t = LittleEndian<int32_t>;
using le_int64_t = LittleEndian<int64_t>;
