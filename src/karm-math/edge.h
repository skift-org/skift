#pragma once

#include "rect.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
union Edge {
    struct {
        Vec2<T> start{}, end{};
    };

    struct {
        T sx, sy, ex, ey;
    };

    T _els[4];

    constexpr Edge() : _els{0, 0, 0, 0} {};

    constexpr Edge(Vec2<T> start, Vec2<T> end) : start(start), end(end) {}

    constexpr Edge(T x1, T y1, T x2, T y2) : start(x1, y1), end(x2, y2) {}

    constexpr Rect<T> bound() const {
        return {start.x, start.y, end.x - start.x, end.y - start.y};
    }

    constexpr Vec2<T> dir() const {
        return end - start;
    }

    constexpr T len() const {
        return dir().len();
    }

    constexpr T operator[](int i) const {
        return _els[i];
    }
};

using Edgei = Edge<int>;

using Edgef = Edge<double>;

} // namespace Karm::Math
