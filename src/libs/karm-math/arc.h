#pragma once

#include "rect.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
union Arc {
    using Scalar = T;

    struct {
        Vec2<T> center{};
        Vec2<T> radii{};
        T start{};
        T end{};
    };

    struct {
        T cx, cy, rx, ry, s, e;
    };

    Array<T, 6> _els;

    constexpr Arc() = default;

    constexpr Arc(T x, T y, T radii, T start, T end)
        : center({x, y}), radii(radii), start(start), end(end) {}

    constexpr Arc(T x, T y, T rx, T ry, T start, T end)
        : center({x, y}), radii({rx, ry}), start(start), end(end) {}

    constexpr Arc(Vec2<T> center, T radii, T start, T end)
        : center(center), radii(radii), start(start), end(end) {}

    constexpr Arc(Vec2<T> center, Vec2<T> radii, T start, T end)
        : center(center), radii(radii), start(start), end(end) {}

    constexpr Arc(Arc const& other)
        : _els{other._els} {}

    constexpr Arc(Arc&& other)
        : _els{std::move(other._els)} {}

    constexpr Arc& operator=(Arc const& other) {
        _els = other._els;
        return *this;
    }

    constexpr Arc& operator=(Arc&& other) {
        _els = std::move(other._els);
        return *this;
    }

    constexpr ~Arc() {
        _els.~Array();
    }

    constexpr Rect<T> bound() const {
        return {
            center.x - radii.x,
            center.y - radii.y,
            radii.x * 2,
            radii.y * 2,
        };
    }

    template <typename U>
    constexpr Arc<U> cast() {
        return {center.template cast<U>(), radii.template cast<U>(), start, end};
    }

    bool hasNan() {
        return center.hasNan() or radii.hasNan() or isNan(start) or isNan(end);
    }

    auto operator<=>(Arc const& other) const {
        return _els <=> other._els;
    }

    bool operator==(Arc const& other) const {
        return _els == other._els;
    }

    void repr(Io::Emit& e) const {
        e("(arc {} {} {} {} {} {})", cx, cy, rx, ry, s, e);
    }

    constexpr auto map(auto f) const {
        using U = decltype(f(cx));

        return Arc<U>{
            f(cx),
            f(cy),
            f(rx),
            f(ry),
            f(s),
            f(e),
        };
    }

    Vec2<T> eval(T t) {
        auto angle = start + (end - start) * t;
        return {
            center.x + radii.x * cos(angle),
            center.y + radii.y * sin(angle),
        };
    }
};

using Arci = Arc<isize>;

using Arcf = Arc<f64>;

} // namespace Karm::Math
