#pragma once

#include "edge.h"
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

    constexpr Tri2 reversed() const {
        return {c, b, a};
    }

    constexpr Vec2<T> min() const {
        return a.min(b).min(c);
    }

    constexpr Vec2<T> max() const {
        return a.max(b).max(c);
    }

    constexpr Rect<T> bound() const {
        return Rect<T>::fromTwoPoint(min(), max());
    }

    constexpr bool degenerated(T epsilon = Limits<T>::EPSILON) const {
        return epsilonEq(a, b, epsilon) or
               epsilonEq(b, c, epsilon) or
               epsilonEq(c, a, epsilon);
    }

    constexpr T turn() const {
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

    constexpr auto map(auto f) const {
        using U = decltype(f(ax));
        return Tri2<U>{f(ax), f(ay), f(bx), f(by), f(cx), f(cy)};
    }
};

using Tri2i = Tri2<isize>;
using Tri2u = Tri2<usize>;
using Tri2f = Tri2<f64>;

} // namespace Karm::Math
