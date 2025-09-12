#pragma once

#include "rect.h"
#include "vec.h"

namespace Karm::Math {

struct Rand {
    u64 _state{};
    u64 _inc{};

    Rand() = default;

    Rand(u64 seed)
        : _state(seed | seed << 32),
          _inc((seed | seed << 32) ^ (seed << 16)) {
    }

    u64 nextU64() {
        return nextU32() | (u64)nextU32() << 32;
    }

    u32 nextU32() {
        u64 oldstate = _state;

        _state = oldstate * 6364136223846793005ULL + (_inc | 1);
        u32 xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
        u32 rot = oldstate >> 59u;

        return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
    }

    u16 nextU16() {
        return nextU32();
    }

    u8 nextU8() {
        return nextU32();
    }

    isize nextInt() {
        return nextU32();
    }

    isize nextInt(isize min, isize max) {
        return min + nextU32() % (max - min);
    }

    isize nextInt(isize max) {
        return nextU32() % max;
    }

    f64 nextDouble() {
        return nextU32() / (f64)0xffffffff;
    }

    f64 nextDouble(f64 min, f64 max) {
        return min + nextU32() / (f64)0xffffffff * (max - min);
    }

    f64 nextDouble(f64 max) {
        return nextU32() / (f64)0xffffffff * max;
    }

    f32 nextFloat() {
        return nextU32() / (f32)0xffffffff;
    }

    f32 nextFloat(f32 min, f32 max) {
        return min + nextU32() / (f32)0xffffffff * (max - min);
    }

    f32 nextFloat(f32 max) {
        return nextU32() / (f32)0xffffffff * max;
    }

    bool nextBool() {
        return nextU32() % 2;
    }

    template <typename T>
    Vec2<T> nextVec2(Rect<T> bound) {
        return Vec2<T>(
            nextDouble(bound.start(), bound.end()),
            nextDouble(bound.top(), bound.bottom())
        );
    }

    template <typename T>
    Vec2<T> nextVec2(Vec2<T> max) {
        return Vec2<T>(nextDouble(max.x), nextDouble(max.y));
    }

    template <typename T>
    T pick(Slice<T> vals) {
        return vals[nextInt(vals.len())];
    }
};

} // namespace Karm::Math
