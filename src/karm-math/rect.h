#pragma once

#include <karm-base/clamp.h>

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

    constexpr Rect() : x(0), y(0), width(0), height(0) {
    }

    constexpr Rect(T x, T y, T width, T height) : x(x), y(y), width(width), height(height) {
    }

    constexpr Rect(Vec2<T> xy, Vec2<T> wh) : xy(xy), wh(wh) {
    }

    constexpr T start() const { return x; }

    constexpr void start(T value) {
        T d = value - x;
        x += d;
        width -= d;
    }

    constexpr T end() const { return x + width; }

    constexpr void end(T value) {
        T d = value - (x + width);
        width += d;
    }

    constexpr T top() const { return y; }

    constexpr void top(T value) {
        T d = value - y;
        y += d;
        height -= d;
    }

    constexpr T bottom() const { return y + height; }

    constexpr void bottom(T value) {
        T d = value - (y + height);
        height += d;
    }

    constexpr Vec2<T> topLeft() const { return {x, y}; }
    constexpr Vec2<T> topRight() const { return {x + width, y}; }
    constexpr Vec2<T> bottomLeft() const { return {x, y + height}; }
    constexpr Vec2<T> bottomRight() const { return {x + width, y + height}; }

    constexpr Vec2<T> center() const { return {x + width / 2, y + height / 2}; }

    constexpr Vec2<T> topCenter() const { return {x + width / 2, y}; }
    constexpr Vec2<T> bottomCenter() const { return {x + width / 2, y + height}; }
    constexpr Vec2<T> leftCenter() const { return {x, y + height / 2}; }
    constexpr Vec2<T> rightCenter() const { return {x + width, y + height / 2}; }

    constexpr Vec2<T> size() const { return {width, height}; }

    constexpr Vec2<T> xw() const { return {x, x + width}; }

    constexpr void xw(Vec2<T> const &v) {
        x = v.x;
        width = v.y - v.x;
    }

    constexpr Vec2<T> yh() const { return {y, y + height}; }

    constexpr void yh(Vec2<T> const &v) {
        y = v.x;
        height = v.y - v.x;
    }

    constexpr bool contains(Vec2<T> const &v) const {
        return v.x >= x && v.y >= y && v.x < x + width && v.y < y + height;
    }

    constexpr bool contains(Rect<T> const &r) const {
        return r.x >= x && r.y >= y && r.x + r.width <= x + width && r.y + r.height <= y + height;
    }

    constexpr bool colide(Rect<T> const &r) const {
        return r.x + r.width > x && r.y + r.height > y && r.x < x + width && r.y < y + height;
    }

    constexpr Rect<T> clipTo(Rect<T> const &r) const {
        return {
            max(x, r.x),
            max(y, r.y),
            min(x + width, r.x + r.width) - max(x, r.x),
            min(y + height, r.y + r.height) - max(y, r.y)};
    }

    constexpr T operator[](int i) { return _els[i]; }

    constexpr T operator[](int i) const { return _els[i]; }
};

using Recti = Rect<int>;

using Rectf = Rect<double>;

} // namespace Karm::Math
