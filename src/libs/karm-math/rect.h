#pragma once

#include <karm-base/clamp.h>
#include <karm-base/cons.h>
#include <karm-fmt/fmt.h>

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

    Array<T, 4> _els;

    ALWAYS_INLINE constexpr Rect()
        : x(0), y(0), width(0), height(0) {
    }

    ALWAYS_INLINE constexpr Rect(T width, T height)
        : x(0), y(0), width(width), height(height) {
    }

    ALWAYS_INLINE constexpr Rect(T x, T y, T width, T height)
        : x(x), y(y), width(width), height(height) {
    }

    ALWAYS_INLINE constexpr Rect(Vec2<T> xy, Vec2<T> wh)
        : xy(xy), wh(wh) {
    }

    ALWAYS_INLINE constexpr Rect(Vec2<T> wh)
        : xy(0), wh(wh) {
    }

    ALWAYS_INLINE static constexpr Rect<T> fromTwoPoint(Vec2<T> a, Vec2<T> b) {
        return {
            min(a.x, b.x),
            min(a.y, b.y),
            max(a.x, b.x) - min(a.x, b.x),
            max(a.y, b.y) - min(a.y, b.y),
        };
    }

    ALWAYS_INLINE constexpr T start() const { return x; }

    ALWAYS_INLINE constexpr void start(T value) {
        T d = value - x;
        x += d;
        width -= d;
    }

    ALWAYS_INLINE constexpr T end() const { return x + width; }

    ALWAYS_INLINE constexpr void end(T value) {
        T d = value - (x + width);
        width += d;
    }

    ALWAYS_INLINE constexpr T top() const { return y; }

    ALWAYS_INLINE constexpr void top(T value) {
        T d = value - y;
        y += d;
        height -= d;
    }

    ALWAYS_INLINE constexpr T bottom() const { return y + height; }

    ALWAYS_INLINE constexpr void bottom(T value) {
        T d = value - (y + height);
        height += d;
    }

    ALWAYS_INLINE constexpr Vec2<T> topStart() const { return {x, y}; }
    ALWAYS_INLINE constexpr Vec2<T> topEnd() const { return {x + width, y}; }
    ALWAYS_INLINE constexpr Vec2<T> bottomStart() const { return {x, y + height}; }
    ALWAYS_INLINE constexpr Vec2<T> bottomEnd() const { return {x + width, y + height}; }

    ALWAYS_INLINE constexpr Vec2<T> center() const { return {x + width / 2, y + height / 2}; }

    ALWAYS_INLINE constexpr Vec2<T> topCenter() const { return {x + width / 2, y}; }
    ALWAYS_INLINE constexpr Vec2<T> bottomCenter() const { return {x + width / 2, y + height}; }
    ALWAYS_INLINE constexpr Vec2<T> startCenter() const { return {x, y + height / 2}; }
    ALWAYS_INLINE constexpr Vec2<T> endCenter() const { return {x + width, y + height / 2}; }

    ALWAYS_INLINE constexpr Vec2<T> size() const { return {width, height}; }

    ALWAYS_INLINE constexpr Vec2<T> xw() const { return {x, x + width}; }

    ALWAYS_INLINE constexpr void xw(Vec2<T> v) {
        x = v.x;
        width = v.y - v.x;
    }

    ALWAYS_INLINE constexpr Vec2<T> yh() const { return {y, y + height}; }

    ALWAYS_INLINE constexpr void yh(Vec2<T> v) {
        y = v.x;
        height = v.y - v.x;
    }

    ALWAYS_INLINE constexpr bool contains(Vec2<T> v) const {
        return v.x >= x and
               v.y >= y and
               v.x < x + width and
               v.y < y + height;
    }

    ALWAYS_INLINE constexpr bool contains(Rect<T> r) const {
        return r.x >= x and
               r.y >= y and
               r.x + r.width <= x + width and
               r.y + r.height <= y + height;
    }

    ALWAYS_INLINE constexpr bool colide(Rect<T> r) const {
        return r.x + r.width > x and
               r.y + r.height > y and
               r.x < x + width and
               r.y < y + height;
    }

    ALWAYS_INLINE constexpr Rect fit(Rect<T> r) const {
        auto scale = (r.size() / size().template cast<f64>()).min();
        Rect result{0, 0, static_cast<T>(width * scale), static_cast<T>(height * scale)};
        result.xy = r.center() - result.center();
        return result;
    }

    ALWAYS_INLINE constexpr Rect cover(Rect<T> r) const {
        f64 scale = (r.size() / size().template cast<f64>()).max();
        Rect result{0, 0, static_cast<T>(width * scale), static_cast<T>(height * scale)};
        result.xy = r.center() - result.center();
        return result;
    }

    ALWAYS_INLINE constexpr Rect center(Rect<T> r) const {
        Rect result{0, 0, width, height};
        result.xy = center() - r.center();
        return result;
    }

    ALWAYS_INLINE constexpr Rect<T> clipTo(Rect<T> r) const {
        if (not colide(r))
            return {};

        return {
            max(x, r.x),
            max(y, r.y),
            min(x + width, r.x + r.width) - max(x, r.x),
            min(y + height, r.y + r.height) - max(y, r.y)};
    }

    ALWAYS_INLINE constexpr Rect<T> mergeWith(Rect<T> r) const {
        return fromTwoPoint(
            {
                min(start(), r.start()),
                min(top(), r.top()),
            },
            {
                max(end(), r.end()),
                max(bottom(), r.bottom()),
            });
    }

    ALWAYS_INLINE constexpr T operator[](isize i) {
        return _els[i];
    }

    ALWAYS_INLINE constexpr T operator[](isize i) const {
        return _els[i];
    }

    template <typename U>
    ALWAYS_INLINE constexpr Rect<U> cast() const {
        return {
            static_cast<U>(x),
            static_cast<U>(y),
            static_cast<U>(width),
            static_cast<U>(height),
        };
    }

    ALWAYS_INLINE Rect<T> ceil() {
        return fromTwoPoint(
            {
                Math::floor(start()),
                Math::floor(top()),
            },
            {
                Math::ceil(end()),
                Math::ceil(bottom()),
            });
    }

    ALWAYS_INLINE Rect<T> floor() {
        return fromTwoPoint(
            {
                Math::ceil(start()),
                Math::ceil(top()),
            },
            {
                Math::floor(end()),
                Math::floor(bottom()),
            });
    }

    ALWAYS_INLINE bool hasNan() const {
        return xy.hasNan() or wh.hasNan();
    }

    ALWAYS_INLINE Cons<Rect, Rect> hsplit(T v) const {
        return {Rect{x, y, v, height}, Rect{x + v, y, width - v, height}};
    }

    ALWAYS_INLINE Cons<Rect, Rect> vsplit(T v) const {
        return {Rect{x, y, width, v}, Rect{x, y + v, width, height - v}};
    }

    ALWAYS_INLINE Rect shrink(Vec2<T> v) const {
        return {x + v.x, y + v.y, width - v.x * 2, height - v.y * 2};
    }

    ALWAYS_INLINE Rect grow(Vec2<T> v) const {
        return {x - v.x, y - v.y, width + v.x * 2, height + v.y * 2};
    }

    ALWAYS_INLINE Rect offset(Vec2<T> v) const {
        return {x + v.x, y + v.y, width, height};
    }
};

using Recti = Rect<isize>;

using Rectu = Rect<usize>;

using Rectf = Rect<f64>;

} // namespace Karm::Math

template <typename T>
ReflectableTemplate$(Math::Rect<T>, x, y, width, height);
