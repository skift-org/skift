#pragma once

// https://github.com/aurimasg/cubic-bezier-offsetter/tree/main

#include <karm-math/vec.h>

namespace Karm::Math {

template <typename T>
struct Curve {
    // Cubic Bezier curve
    Vec2<T> a, b, c, d;

    static Curve cubic(Vec2<T> a, Vec2<T> b, Vec2<T> c, Vec2<T> d) {
        return {a, b, c, d};
    }

    static Curve quadratic(Vec2<T> a, Vec2<T> b, Vec2<T> c) {
        return {a, a * 2 / 3 + b / 3, c * 2 / 3 + b / 3, c};
    }

    static Curve linear(Vec2<T> a, Vec2<T> b) {
        return quadratic(a, (a + b) / 2, b);
    }

    bool isPoint(T epsilon = Limits<T>::EPSILON) const {
        return epsilonEq(a, b, epsilon) and
               epsilonEq(b, c, epsilon) and
               epsilonEq(c, d, epsilon);
    }
};

using Curvei = Curve<isize>;

using Curveu = Curve<usize>;

using Curvef = Curve<f64>;

} // namespace Karm::Math

template <typename T>
ReflectableTemplate$(Math::Curve<T>, a, b, c, d);
