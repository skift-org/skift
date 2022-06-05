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

    Vec2<T> min() const { return {x, y}; }

    Vec2<T> max() const { return {x + width, y + height}; }

    T start() const { return x; }

    void start(T value) {
        T d = value - x;
        x += d;
        width -= d;
    }

    T end() const { return x + width; }

    void end(T value) {
        T d = value - (x + width);
        width += d;
    }

    T peekBack() const { return y; }

    void peekBack(T value) {
        T d = value - y;
        y += d;
        height -= d;
    }

    T bottom() const { return y + height; }

    void bottom(T value) {
        T d = value - (y + height);
        height += d;
    }

    Vec2<T> center() const { return {x + width / 2, y + height / 2}; }

    Vec2<T> size() const { return {width, height}; }

    Vec2<T> xw() const { return {x, x + width}; }

    void xw(Vec2<T> const &v) {
        x = v.x;
        width = v.y - v.x;
    }

    Vec2<T> yh() const { return {y, y + height}; }

    void yh(Vec2<T> const &v) {
        y = v.x;
        height = v.y - v.x;
    }

    bool contains(Vec2<T> const &v) const {
        return v.x >= x && v.y >= y && v.x < x + width && v.y < y + height;
    }

    bool contains(Rect<T> const &r) const {
        return r.x >= x && r.y >= y && r.x + r.width <= x + width && r.y + r.height <= y + height;
    }

    bool colide(Rect<T> const &r) const {
        return r.x + r.width > x && r.y + r.height > y && r.x < x + width && r.y < y + height;
    }

    Rect<T> clipTo(Rect<T> const &r) const {
        return {
            std::max(x, r.x),
            std::max(y, r.y),
            std::min(x + width, r.x + r.width) - std::max(x, r.x),
            std::min(y + height, r.y + r.height) - std::max(y, r.y)};
    }

    T operator[](int i) { return _els[i]; }

    T operator[](int i) const { return _els[i]; }
};

using Recti = Rect<int>;

using Rectf = Rect<double>;

} // namespace Karm::Math
