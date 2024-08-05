#pragma once

#include "vec.h"

namespace Karm::Math {

template <typename T>
union Tri2 {
    enum struct Orien {
        CLOCKWISE,
        COUNTER_CLOCKWISE,
        COLLINEAR,
    };

    using Scalar = T;

    struct {
        T ax, ay, bx, by, cx, cy;
    };

    struct {
        Vec2<T> a, b, c;
    };

    Array<T, 6> _els;
    Array<Vec2<T>, 3> _pts;

    constexpr Tri2()
        : _els{} {}

    constexpr Tri2(T ax, T ay, T bx, T by, T cx, T cy)
        : ax{ax}, ay{ay}, bx{bx}, by{by}, cx{cx}, cy{cy} {}

    constexpr Tri2(Vec2<T> a, Vec2<T> b, Vec2<T> c)
        : a{a}, b{b}, c{c} {}

    T turn() const {
        return (b - a).cross(c - a);
    }

    Orien orien() const {
        auto t = turn();
        if (t > 0)
            return Orien::CLOCKWISE;
        if (t < 0)
            return Orien::COUNTER_CLOCKWISE;
        return Orien::COLLINEAR;
    }

    void repr(Io::Emit &e) const {
        e("(tri {} {} {})", a, b, c);
    }
};

using Tri2i = Tri2<isize>;
using Tri2u = Tri2<usize>;
using Tri2f = Tri2<f64>;

} // namespace Karm::Math
