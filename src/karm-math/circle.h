#pragma once

#include "rect.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
struct Circle {
    Vec2<T> center{};
    T radius{};

    constexpr Circle() = default;

    constexpr Circle(Vec2<T> center, T radius) : center(center), radius(radius) {}

    constexpr Circle(T x, T y, T radius) : center(x, y), radius(radius) {}

    constexpr Rect<T> bound() const {
        return {center.x - radius, center.y - radius, radius * 2, radius * 2};
    }

    template <typename V>
    constexpr bool contains(Vec2<V> const &point) const {
        return center.dist(point) <= radius;
    }
};

using Circlei = Circle<int>;

using Circlef = Circle<double>;

} // namespace Karm::Math
