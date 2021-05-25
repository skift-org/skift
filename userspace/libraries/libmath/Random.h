#pragma once

#include <libutils/Prelude.h>
#include <skift/Time.h>

namespace Math
{

struct Random
{
private:
    uint64_t _state;
    uint64_t _inc;

public:
    void seed(uint32_t seed)
    {
        _state = seed | (uint64_t)seed << 32;
        _inc = (seed | ((uint64_t)seed << 32)) ^ ((uint64_t)seed << 16);
    }

    Random()
    {
        seed(timestamp_now());
    }

    Random(int32_t seed)
    {
        Random::seed(seed);
    }

    uint8_t next_u8()
    {
        return (uint8_t)next_u32();
    }

    uint16_t next_u16()
    {
        return (uint16_t)next_u32();
    }

    uint32_t next_u32()
    {
        uint64_t oldstate = _state;

        // Advance internal state
        _state = oldstate * 6364136223846793005ULL + (_inc | 1);

        // Calculate output function (XSH RR), uses old state for max ILP
        uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
        uint32_t rot = oldstate >> 59u;

        return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
    }

    uint32_t next_u32(uint32_t max)
    {
        return next_u32() % max;
    }

    uint64_t next_u64()
    {
        return (uint64_t)next_u32() << 32 | next_u32();
    }

    template <typename T>
    T pick(T values[], size_t count)
    {
        return values[next_u32(count)];
    }

#ifndef __KERNEL__

    double next_double()
    {
        return next_u32() / (double)UINT32_MAX;
    }

    double next_double_minus_one_to_one()
    {
        return (next_double() * 2) - 1;
    }

#endif
};

} // namespace Math
