#pragma once

#include "rect.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
union Ellipse {
    struct {
        Vec2<T> center{};
        Vec2<T> radius{};
    };

    struct {
        T cx, cy, rx, ry;
    };

    Array<T, 4> _els;

    constexpr Ellipse() = default;

    constexpr Ellipse(T x, T y, T radius) : center({x, y}), radius(radius) {}

    constexpr Ellipse(T x, T y, T rx, T ry) : center({x, y}), radius({rx, ry}) {}

    constexpr Ellipse(Vec2<T> center, T radius) : center(center), radius(radius) {}

    constexpr Ellipse(Vec2<T> center, Vec2<T> radius) : center(center), radius(radius) {}

    constexpr Rect<T> bound() const {
        return {center.x - radius.x, center.y - radius.y, radius.x * 2, radius.y * 2};
    }

    template <typename U>
    constexpr Ellipse<U> cast() {
        return {center.template cast<U>(), radius.template cast<U>()};
    }

    bool hasNan() {
        return center.hasNan() or radius.hasNan();
    }
};

using Ellipsei = Ellipse<isize>;

using Ellipsef = Ellipse<f64>;

} // namespace Karm::Math

template <typename T>
ReflectableTemplate$(Math::Ellipse<T>, cx, cy, rx, ry);
