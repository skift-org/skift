#pragma once

// https://github.com/aurimasg/cubic-bezier-offsetter/tree/main
// https://pomax.github.io/bezierinfo/#offsetting

#include "vec.h"

namespace Karm::Math {

template <typename T>
union Curve {
    // Cubic Bezier curve
    struct {
        Vec2<T> a, b, c, d;
    };

    struct {
        T ax, ay, bx, by, cx, cy, dx, dy;
    };

    Array<T, 8> _els;

    Array<Vec2<T>, 4> _pts;

    Curve()
        : _els{} {}

    Curve(Vec2<T> a, Vec2<T> b, Vec2<T> c, Vec2<T> d)
        : a(a), b(b), c(c), d(d) {}

    static Curve cubic(Vec2<T> a, Vec2<T> b, Vec2<T> c, Vec2<T> d) {
        return {a, b, c, d};
    }

    static Curve quadratic(Vec2<T> a, Vec2<T> b, Vec2<T> c) {
        return {a, a * 2 / 3 + b / 3, c * 2 / 3 + b / 3, c};
    }

    static Curve linear(Vec2<T> a, Vec2<T> b) {
        return quadratic(a, (a + b) / 2, b);
    }

    bool degenerated(T epsilon = Limits<T>::EPSILON) const {
        return epsilonEq(a, b, epsilon) and
               epsilonEq(b, c, epsilon) and
               epsilonEq(c, d, epsilon);
    }

    bool straight(T epsilon = Limits<T>::EPSILON) const {
        auto minx = min(ax, dy);
        auto miny = min(ay, dy);
        auto maxx = max(ax, dx);
        auto maxy = max(ay, dy);

        return
            // Is P1 located between P0 and P3?
            minx <= bx and
            miny <= by and
            maxx >= bx and
            maxy >= by and
            // Is P2 located between P0 and P3?
            minx <= cx and
            miny <= cy and
            maxx >= cx and
            maxy >= cy and
            // Are all points collinear?
            epsilonEq(Tri2(a, b, d).turn(), 0.0, epsilon) and
            epsilonEq(Tri2(a, c, d).turn(), 0.0, epsilon);
    }

    Vec2<T> eval(T t) const {
        auto u = 1 - t;
        auto uu = u * u;
        auto uuu = uu * u;
        auto tt = t * t;
        auto ttt = tt * t;

        return a * uuu + b * 3 * uu * t + c * 3 * u * tt + d * ttt;
    }

    Vec2<T> derivative(T t) const {
        auto u = 1 - t;
        auto uu = u * u;
        auto tt = t * t;
        return (b - a) * 3 * uu + (c - b) * 6 * u * t + (d - c) * 3 * tt;
    }

    Vec2<T> derivative2(T t) const {
        auto u = 1 - t;
        return (c - b) * 6 * u + (d - c) * 6 * t;
    }

    Vec2<T> normal(T t) const {
        auto d = derivative(t);
        return Vec2<T>{-d.y, d.x}.norm();
    }

    Curve offset(T offset) const {
        Curve res;
        for (usize i = 0; i < 4; i++) {
            res._pts[i] = _pts[i] + normal(i / 3.0) * offset;
        }
        return res;
    }
};

using Curvei = Curve<isize>;

using Curveu = Curve<usize>;

using Curvef = Curve<f64>;

} // namespace Karm::Math

template <typename T>
ReflectableTemplate$(Math::Curve<T>, a, b, c, d);
