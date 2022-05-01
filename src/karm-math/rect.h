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

    Rect() : x(0), y(0), width(0), height(0) {
    }

    Rect(T x, T y, T width, T height) : x(x), y(y), width(width), height(height) {
    }

    Rect(Vec2<T> xy, Vec2<T> wh) : xy(xy), wh(wh) {
    }

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

    auto contains(Vec2<T> const &v) const -> bool {
        return v.x >= x && v.y >= y && v.x < x + width && v.y < y + height;
    }

    auto contains(Rect<T> const &r) const -> bool {
        return r.x >= x && r.y >= y && r.x + r.width <= x + width && r.y + r.height <= y + height;
    }

    auto colide(Rect<T> const &r) const -> bool {
        return r.x + r.width > x && r.y + r.height > y && r.x < x + width && r.y < y + height;
    }

    auto operator[](int i) -> T { return _els[i]; }

    auto operator[](int i) const -> T { return _els[i]; }
};

using Recti = Rect<int>;

using Rectf = Rect<double>;

} // namespace Karm::Math
