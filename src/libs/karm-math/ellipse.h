#pragma once

#include "rect.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
struct Ellipse {
    Vec2<T> center{};
    Vec2<T> radius{};

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
struct Karm::Fmt::Formatter<Math::Ellipse<T>> {
    Res<usize> format(Io::TextWriter &writer, Math::Ellipse<T> ellipse) {
        return Fmt::format(writer, "Ellipse({}, {}, {}, {})", ellipse.center.x, ellipse.center.y, ellipse.radius.x, ellipse.radius.y);
    }
};
