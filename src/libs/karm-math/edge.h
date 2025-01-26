#pragma once

#include "rect.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
union Edge {
    using Scalar = T;

    struct {
        Vec2<T> start{}, end{};
    };

    struct {
        T sx, sy, ex, ey;
    };

    Array<T, 4> _els;

    constexpr Edge()
        : _els{} {};

    constexpr Edge(Vec2<T> start, Vec2<T> end)
        : start(start), end(end) {}

    constexpr Edge(T x1, T y1, T x2, T y2)
        : start(x1, y1), end(x2, y2) {}

    constexpr Edge(Edge const& other)
        : _els{other._els} {}

    constexpr Edge(Edge&& other)
        : _els{std::move(other._els)} {}

    constexpr Edge& operator=(Edge const& other) {
        _els = other._els;
        return *this;
    }

    constexpr Edge& operator=(Edge&& other) {
        _els = std::move(other._els);
        return *this;
    }

    constexpr ~Edge() {
        _els.~Array();
    }

    constexpr Edge reversed() const {
        return {end, start};
    }

    constexpr bool degenerated(T epsilon = Limits<T>::EPSILON) const {
        return epsilonEq(start, end, epsilon);
    }

    constexpr Vec<T> delta() const {
        return {end.x - start.x, end.y - start.y};
    }

    constexpr Rect<T> bound() const {
        return Rect<T>::fromTwoPoint(start, end);
    }

    constexpr Edge<T> offset(T offset) {
        auto d = end - start;
        f64 scale = offset / d.len();
        Vec2<T> o = {-d.y * scale, d.x * scale};
        auto s = start + o;
        auto e = end + o;
        return {s, e};
    }

    constexpr Vec2<T> dir() const {
        return end - start;
    }

    constexpr Vec2<T> invDir() const {
        return start - end;
    }

    constexpr T len() const {
        return dir().len();
    }

    constexpr T lenSq() const {
        return dir().lenSq();
    }

    constexpr T operator[](isize i) const {
        return _els[i];
    }

    constexpr Edge operator+(Vec2<T> other) const {
        return {start + other, end + other};
    }

    constexpr Edge operator-(Vec2<T> other) const {
        return {start - other, end - other};
    }

    template <typename U>
    constexpr Edge<U> cast() const {
        return {
            start.template cast<U>(),
            end.template cast<U>(),
        };
    }

    constexpr Edge<T> swap() const {
        return {end, start};
    }

    constexpr bool hasNan() const {
        return start.hasNan() or end.hasNan();
    }

    void repr(Io::Emit& e) const {
        e("(edge {} {} {} {})", sx, sy, ex, ey);
    }

    constexpr auto map(auto f) const {
        using U = decltype(f(sx));
        return Edge<U>{f(sx), f(sy), f(ex), f(ey)};
    }
};

using Edgei = Edge<isize>;

using Edgef = Edge<f64>;

} // namespace Karm::Math
