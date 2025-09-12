#pragma once

#include "rect.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
union Ellipse {
    using Scalar = T;

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

    constexpr Ellipse(Ellipse const& other)
        : _els{other._els} {}

    constexpr Ellipse(Ellipse&& other)
        : _els{std::move(other._els)} {}

    constexpr Ellipse& operator=(Ellipse const& other) {
        _els = other._els;
        return *this;
    }

    constexpr Ellipse& operator=(Ellipse&& other) {
        _els = std::move(other._els);
        return *this;
    }

    constexpr ~Ellipse() {
        _els.~Array();
    }

    constexpr Rect<T> bound() const {
        return {center.x - radii.x, center.y - radii.y, radii.x * 2, radii.y * 2};
    }

    template <typename U>
    constexpr Ellipse<U> cast() const {
        return {center.template cast<U>(), radii.template cast<U>()};
    }

    bool hasNan() {
        return center.hasNan() or radii.hasNan();
    }

    void repr(Io::Emit& e) const {
        e("(ellipse {} {} {} {})", cx, cy, rx, ry);
    }

    constexpr auto map(auto f) const {
        using U = decltype(f(cx));
        return Ellipse<U>{f(cx), f(cy), f(rx), f(ry)};
    }
};

using Ellipsei = Ellipse<isize>;

using Ellipsef = Ellipse<f64>;

} // namespace Karm::Math
