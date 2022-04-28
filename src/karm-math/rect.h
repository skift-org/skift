#pragma once

#include "vec.h"

namespace Karm::Math {

template <typename T>
union Rect {
    struct {
        T x, y, width, height;
    };

    struct {
        Vec2<T> xy;
        Vec2<T> wh;
    };

    T _els[4];

    auto min() const -> Vec2<T> { return {x, y}; }

    auto max() const -> Vec2<T> { return {x + width, y + height}; }

    auto start() const -> T { return x; }

    void start(T value) {
        T d = value - x;
        x += d;
        width -= d;
    }

    auto end() const -> T { return x + width; }

    void end(T value) {
        T d = value - (x + width);
        width += d;
    }

    auto top() const -> T { return y; }

    void top(T value) {
        T d = value - y;
        y += d;
        height -= d;
    }

    auto bottom() const -> T { return y + height; }

    void bottom(T value) {
        T d = value - (y + height);
        height += d;
    }

    auto center() const -> Vec2<T> { return {x + width / 2, y + height / 2}; }

    auto size() const -> Vec2<T> { return {width, height}; }

    auto xw() const -> Vec2<T> { return {x, x + width}; }

    void xw(Vec2<T> const &v) {
        x = v.x;
        width = v.y - v.x;
    }

    auto yh() const -> Vec2<T> { return {y, y + height}; }

    void yh(Vec2<T> const &v) {
        y = v.x;
        height = v.y - v.x;
    }

    auto operator[](int i) -> T { return _els[i]; }

    auto operator[](int i) const -> T { return _els[i]; }
};

} // namespace Karm::Math
