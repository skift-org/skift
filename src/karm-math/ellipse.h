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
};

using Ellipsei = Ellipse<int>;

using Ellipsef = Ellipse<double>;

} // namespace Karm::Math

namespace Karm::Fmt {

template <typename T>
struct Formatter<Math::Ellipse<T>> {
    Result<size_t> format(Io::_TextWriter &writer, Math::Ellipse<T> ellipse) {
        return Fmt::format(writer, "Ellipse({}, {}, {}, {})", ellipse.center.x, ellipse.center.y, ellipse.radius.x, ellipse.radius.y);
    }
};

} // namespace Karm::Fmt
