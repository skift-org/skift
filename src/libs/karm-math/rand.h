#pragma once

#include <karm-base/std.h>

#include "rect.h"
#include "vec.h"

namespace Karm::Math {

struct Rand {
    uint64_t _state{};
    uint64_t _inc{};

    Rand() = default;

    Rand(int seed) : Rand((uint64_t)seed) {}

    Rand(uint64_t seed)
        : _state(seed | seed << 32),
          _inc((seed | seed << 32) ^ (seed << 16)) {
    }

    uint64_t nextU64() {
        return nextU32() | (uint64_t)nextU32() << 32;
    }

    uint32_t nextU32() {
        uint64_t oldstate = _state;

        _state = oldstate * 6364136223846793005ULL + (_inc | 1);
        uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
        uint32_t rot = oldstate >> 59u;

        return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
    }

    uint16_t nextU16() {
        return nextU32();
    }

    uint8_t nextU8() {
        return nextU32();
    }

    int nextInt() {
        return nextU32();
    }

    int nextInt(int min, int max) {
        return min + nextU32() % (max - min + 1);
    }

    int nextInt(int max) {
        return nextU32() % (max + 1);
    }

    double nextDouble() {
        return nextU32() / (double)0xffffffff;
    }

    double nextDouble(double min, double max) {
        return min + nextU32() / (double)0xffffffff * (max - min);
    }

    double nextDouble(double max) {
        return nextU32() / (double)0xffffffff * max;
    }

    float nextFloat() {
        return nextU32() / (float)0xffffffff;
    }

    float nextFloat(float min, float max) {
        return min + nextU32() / (float)0xffffffff * (max - min);
    }

    float nextFloat(float max) {
        return nextU32() / (float)0xffffffff * max;
    }

    bool nextBool() {
        return nextU32() % 2;
    }

    template <typename T>
    Vec2<T> nextVec2(Rect<T> bound) {
        return Vec2<T>(nextDouble(bound.start(), bound.end()),
                       nextDouble(bound.top(), bound.bottom()));
    }
};

} // namespace Karm::Math
