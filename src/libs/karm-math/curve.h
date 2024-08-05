#pragma once

// Yang, S.-N., & Huang, M.-L. (1993). A New Shape Control and Classification
// for Cubic Bézier Curves. Communicating with Virtual Worlds, 204–215.
// doi:10.1007/978-4-431-68456-5_17 
// https://github.com/aurimasg/cubic-bezier-offsetter/tree/main
// https://github.com/Pomax/BezierInfo-2/tree/master
// https://pomax.github.io/bezierinfo/#offsetting
// https://gasiulis.name/cubic-curve-offsetting/

#include "tri.h"
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

    constexpr Curve()
        : _els{} {}

    constexpr Curve(Vec2<T> a, Vec2<T> b, Vec2<T> c, Vec2<T> d)
        : a(a), b(b), c(c), d(d) {}

    static constexpr Curve cubic(Vec2<T> a, Vec2<T> b, Vec2<T> c, Vec2<T> d) {
        return {a, b, c, d};
    }

    static constexpr Curve quadratic(Vec2<T> a, Vec2<T> b, Vec2<T> c) {
        return {a, a * 2 / 3 + b / 3, c * 2 / 3 + b / 3, c};
    }

    static constexpr Curve linear(Vec2<T> a, Vec2<T> b) {
        return quadratic(a, (a + b) / 2, b);
    }

    constexpr bool degenerated(T epsilon = Limits<T>::EPSILON) const {
        return epsilonEq(a, b, epsilon) and
               epsilonEq(b, c, epsilon) and
               epsilonEq(c, d, epsilon);
    }

    constexpr bool straight(T epsilon = Limits<T>::EPSILON) const {
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

    constexpr bool straightish(T tolerance) {
        auto d1 = d - a;
        auto d2 = Math::abs((b.x - d.x) * d1.y - (b.y - d.y) * d1.x);
        auto d3 = Math::abs((c.x - d.x) * d1.y - (c.y - d.y) * d1.x);

        return (d2 + d3) * (d2 + d3) < tolerance * (d1.x * d1.x + d1.y * d1.y);
    }

    constexpr Vec2<T> eval(T t) const {
        auto u = 1 - t;
        auto uu = u * u;
        auto uuu = uu * u;
        auto tt = t * t;
        auto ttt = tt * t;

        return a * uuu + b * 3 * uu * t + c * 3 * u * tt + d * ttt;
    }

    constexpr Vec2<T> derivative(T t) const {
        auto u = 1 - t;
        auto uu = u * u;
        auto tt = t * t;

        return (b - a) * 3 * uu + (c - b) * 6 * u * t + (d - c) * 3 * tt;
    }

    constexpr Vec2<T> derivative2(T t) const {
        auto u = 1 - t;

        return (c - b) * 6 * u + (d - c) * 6 * t;
    }

    constexpr Vec2<T> normal(T t) const {
        return derivative(t).normal();
    }

    constexpr Curve offset(T offset) const {
        Vec2<T> d1 = b - a;
        Vec2<T> d2 = c - d;
        auto div = d1.cross(d2);

        if (epsilonEq(div, 0.0))
            return offset2(offset);

        // Start point.
        Vec2<T> p0 = a + normal(0.0).unit() * offset;

        // End point.
        Vec2<T> p3 = d + normal(1.0).unit() * offset;

        // Middle point.
        Vec2<T> mp = eval(0.5);
        Vec2<T> mpN = normal(0.5).unit();
        Vec2<T> p = mp + (mpN * offset);

        Vec2<T> bxby = (8.0 / 3.0) * (p - (0.5 * (p0 + p3)));

        auto a = bxby.cross(d2) / div;
        auto b = d1.cross(bxby) / div;

        Vec2<T> p1{p0.x + a * d1.x, p0.y + a * d1.y};
        Vec2<T> p2{p3.x + b * d2.x, p3.y + b * d2.y};

        return {p0, p1, p2, p3};
    }

    constexpr Pair<Curve> split(T t = 0.5) const {
        auto e = lerp(a, b, t);
        auto f = lerp(b, c, t);
        auto g = lerp(c, d, t);
        auto h = lerp(e, f, t);
        auto j = lerp(f, g, t);
        auto k = lerp(h, j, t);

        return {
            {a, e, h, k},
            {k, j, g, d},
        };
    }

    constexpr Curve offset2(T offset) const {
        Curve res;
        for (usize i = 0; i < 4; i++)
            res._pts[i] = _pts[i] + normal(i / 3.0).unit() * offset;
        return res;
    }

    void repr(Io::Emit &e) const {
        e("(curve {} {} {} {})", a, b, c, d);
    }
};

using Curvei = Curve<isize>;

using Curveu = Curve<usize>;

using Curvef = Curve<f64>;

} // namespace Karm::Math
