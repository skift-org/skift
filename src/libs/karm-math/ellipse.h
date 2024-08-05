#pragma once

#include "rect.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
union Ellipse {
    struct {
        Vec2<T> center{};
        Vec2<T> radii{};
    };

    struct {
        T cx, cy, rx, ry;
    };

    Array<T, 4> _els;

    constexpr Ellipse() = default;

    constexpr Ellipse(T x, T y, T radii) : center({x, y}), radii(radii) {}

    constexpr Ellipse(T x, T y, T rx, T ry) : center({x, y}), radii({rx, ry}) {}

    constexpr Ellipse(Vec2<T> center, T radii) : center(center), radii(radii) {}

    constexpr Ellipse(Vec2<T> center, Vec2<T> radii) : center(center), radii(radii) {}

    constexpr Rect<T> bound() const {
        return {center.x - radii.x, center.y - radii.y, radii.x * 2, radii.y * 2};
    }

    template <typename U>
    constexpr Ellipse<U> cast() {
        return {center.template cast<U>(), radii.template cast<U>()};
    }

    bool hasNan() {
        return center.hasNan() or radii.hasNan();
    }

    void repr(Io::Emit &e) const {
        e("(ellipse {} {} {} {})", cx, cy, rx, ry);
    }
};

using Ellipsei = Ellipse<isize>;

using Ellipsef = Ellipse<f64>;

} // namespace Karm::Math
