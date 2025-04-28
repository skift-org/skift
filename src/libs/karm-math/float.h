#pragma once

#include <karm-base/base.h>

#include "const.h"

namespace Karm::Math {

template <typename T>
union FloatBits;

template <>
union FloatBits<f16> {
    static constexpr int mantissaBits = 10;
    static constexpr u16 mantissaMax = (((u16)1) << 10) - 1;
    static constexpr int exponentBias = 15;
    static constexpr int exponentBits = 5;
    static constexpr unsigned exponentMax = 31;

    struct [[gnu::packed]] {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        u16 sign : 1;
        u16 exponent : 5;
        u16 mantissa : 10;
#else
        u16 mantissa : 10;
        u16 exponent : 5;
        u16 sign : 1;
#endif
    };

    f16 d;
};

static_assert(sizeof(f16) == 2);
static_assert(sizeof(FloatBits<f16>) == sizeof(f16));

template <>
union FloatBits<f32> {
    static constexpr int mantissaBits = 23;
    static constexpr u32 mantissaMax = (((u32)1) << 23) - 1;
    static constexpr int exponentBias = 127;
    static constexpr int exponentBits = 8;
    static constexpr unsigned exponentMax = 255;

    struct [[gnu::packed]] {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        u32 sign : 1;
        u32 exponent : 8;
        u32 mantissa : 23;
#else
        u32 mantissa : 23;
        u32 exponent : 8;
        u32 sign : 1;
#endif
    };

    f32 d;
};

static_assert(sizeof(f32) == 4);
static_assert(sizeof(FloatBits<f32>) == sizeof(f32));

template <>
union FloatBits<f64> {
    static constexpr int mantissaBits = 52;
    static constexpr u64 mantissaMax = (((u64)1) << 52) - 1;
    static constexpr int exponentBias = 1023;
    static constexpr int exponentBits = 11;
    static constexpr unsigned exponentMax = 2047;

    struct [[gnu::packed]] {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        u64 sign : 1;
        u64 exponent : 11;
        u64 mantissa : 52;
#else
        u64 mantissa : 52;
        u64 exponent : 11;
        u64 sign : 1;
#endif
    };

    f64 d;
};

static_assert(sizeof(f64) == 8);
static_assert(sizeof(FloatBits<f64>) == sizeof(f64));

#ifdef __SIZEOF_FLOAT128__

template <>
union FloatBits<f128> {
    static constexpr int mantissaBits = 112;
    static constexpr u128 mantissaMax = (((u128)1) << 112) - 1;
    static constexpr int exponentBias = 16383;
    static constexpr int exponentBits = 15;
    static constexpr unsigned exponentMax = 32767;

    struct [[gnu::packed]] {
#    if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        u128 sign : 1;
        u128 exponent : 15;
        u128 mantissa : 112;
#    else
        u128 mantissa : 112;
        u128 exponent : 15;
        u128 sign : 1;
#    endif
    };

    f128 d;
};

static_assert(sizeof(f128) == 16);
static_assert(sizeof(FloatBits<f128>) == sizeof(f128));

#endif

} // namespace Karm::Math

template <typename T>
    requires requires { Math::FloatBits<T>::mantissaBits; }
struct Karm::Niche<T> {
    struct Content {
        Math::FloatBits<T> bits;

        constexpr Content() {
            bits.d = Math::NAN;
            bits.mantissa &= ~1;
        }

        constexpr bool has() const {
            return not __builtin_isnan(bits.d) or bits.mantissa & 1;
        }

        constexpr void setupValue() {
            if (__builtin_isnan(bits.d)) {
                bits.mantissa |= 1;
            }
        }
    };
};
