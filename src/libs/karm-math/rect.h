#pragma once

#include <karm-base/clamp.h>
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

    constexpr Rect()
        : x(0), y(0), width(0), height(0) {
    }

    constexpr Rect(T width, T height)
        : x(0), y(0), width(width), height(height) {
    }

    constexpr Rect(T x, T y, T width, T height)
        : x(x), y(y), width(width), height(height) {
    }

    constexpr Rect(Vec2<T> xy, Vec2<T> wh)
        : xy(xy), wh(wh) {
    }

    constexpr Rect(Vec2<T> wh)
        : xy(0), wh(wh) {
    }

    static constexpr Rect<T> fromTwoPoint(Vec2<T> a, Vec2<T> b) {
        return {
            min(a.x, b.x),
            min(a.y, b.y),
            max(a.x, b.x) - min(a.x, b.x),
            max(a.y, b.y) - min(a.y, b.y),
        };
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

    constexpr Vec2<T> topStart() const { return {x, y}; }
    constexpr Vec2<T> topEnd() const { return {x + width, y}; }
    constexpr Vec2<T> bottomStart() const { return {x, y + height}; }
    constexpr Vec2<T> bottomEnd() const { return {x + width, y + height}; }

    constexpr Vec2<T> center() const { return {x + width / 2, y + height / 2}; }

    constexpr Vec2<T> topCenter() const { return {x + width / 2, y}; }
    constexpr Vec2<T> bottomCenter() const { return {x + width / 2, y + height}; }
    constexpr Vec2<T> startCenter() const { return {x, y + height / 2}; }
    constexpr Vec2<T> endCenter() const { return {x + width, y + height / 2}; }

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
        return v.x >= x and
               v.y >= y and
               v.x < x + width and
               v.y < y + height;
    }

    constexpr bool contains(Rect<T> const &r) const {
        return r.x >= x and
               r.y >= y and
               r.x + r.width <= x + width and
               r.y + r.height <= y + height;
    }

    constexpr bool colide(Rect<T> const &r) const {
        return r.x + r.width > x and
               r.y + r.height > y and
               r.x < x + width and
               r.y < y + height;
    }

    constexpr Rect fit(Rect<T> const &r) const {
        auto scale = (r.size() / size().template cast<double>()).min();
        Rect result{0, 0, static_cast<T>(width * scale), static_cast<T>(height * scale)};
        result.xy = r.center() - result.center();
        return result;
    }

    constexpr Rect cover(Rect<T> const &r) const {
        double scale = (r.size() / size().template cast<double>()).max();
        Rect result{0, 0, static_cast<T>(width * scale), static_cast<T>(height * scale)};
        result.xy = r.center() - result.center();
        return result;
    }

    constexpr Rect center(Rect<T> const &r) const {
        Rect result{0, 0, width, height};
        result.xy = center() - r.center();
        return result;
    }

    constexpr Rect<T> clipTo(Rect<T> const &r) const {
        return {
            max(x, r.x),
            max(y, r.y),
            min(x + width, r.x + r.width) - max(x, r.x),
            min(y + height, r.y + r.height) - max(y, r.y)};
    }

    constexpr Rect<T> mergeWith(Rect<T> const &r) const {
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

    constexpr T operator[](int i) {
        return _els[i];
    }

    constexpr T operator[](int i) const {
        return _els[i];
    }

    template <typename U>
    constexpr Rect<U> cast() const {
        return {
            static_cast<U>(x),
            static_cast<U>(y),
            static_cast<U>(width),
            static_cast<U>(height),
        };
    }

    Rect<T> ceil() {
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

    Rect<T> floor() {
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

    bool hasNan() const {
        return xy.hasNan() or wh.hasNan();
    }

    Cons<Rect, Rect> hsplit(T v) const {
        return {Rect{x, y, v, height}, Rect{x + v, y, width - v, height}};
    }

    Cons<Rect, Rect> vsplit(T v) const {
        return {Rect{x, y, width, v}, Rect{x, y + v, width, height - v}};
    }

    Rect shrink(Vec2<T> v) const {
        return {x + v.x, y + v.y, width - v.x * 2, height - v.y * 2};
    }

    Rect grow(Vec2<T> v) const {
        return {x - v.x, y - v.y, width + v.x * 2, height + v.y * 2};
    }
};

using Recti = Rect<int>;

using Rectf = Rect<double>;

} // namespace Karm::Math

template <typename T>
struct Karm::Fmt::Formatter<Math::Rect<T>> {
    Result<size_t> format(Io::_TextWriter &writer, Math::Rect<T> rect) {
        return Fmt::format(writer, "Rect({}, {}, {}, {})", rect.x, rect.y, rect.width, rect.height);
    }
};
