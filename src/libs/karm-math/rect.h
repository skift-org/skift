#pragma once

#include <karm-base/clamp.h>
#include <karm-base/tuple.h>

#include "insets.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
union Rect {
    using Scalar = T;

    struct {
        T x, y, width, height;
    };

    struct {
        Vec2<T> xy;
        Vec2<T> wh;
    };

    Array<T, 4> _els;

    static Rect const ZERO;

    static Rect const ONE;

    static Rect const MAX;

    always_inline constexpr Rect()
        : x(), y(), width(), height() {
    }

    always_inline constexpr Rect(T width, T height)
        : x(), y(), width(width), height(height) {
    }

    always_inline constexpr Rect(T x, T y, T width, T height)
        : x(x), y(y), width(width), height(height) {
    }

    always_inline constexpr Rect(Vec2<T> xy, Vec2<T> wh)
        : xy(xy), wh(wh) {
    }

    always_inline constexpr Rect(Vec2<T> wh)
        : xy(), wh(wh) {
    }

    constexpr Rect(Rect const& other)
        : _els{other._els} {}

    constexpr Rect(Rect&& other)
        : _els{std::move(other._els)} {}

    constexpr Rect& operator=(Rect const& other) {
        _els = other._els;
        return *this;
    }

    constexpr Rect& operator=(Rect&& other) {
        _els = std::move(other._els);
        return *this;
    }

    constexpr ~Rect() {
        _els.~Array();
    }

    always_inline static constexpr Rect<T> fromTwoPoint(Vec2<T> a, Vec2<T> b) {
        return {
            min(a.x, b.x),
            min(a.y, b.y),
            max(a.x, b.x) - min(a.x, b.x),
            max(a.y, b.y) - min(a.y, b.y),
        };
    }

    always_inline static constexpr Rect<T> fromCenter(Vec2<T> center, Vec2<T> size) {
        return {
            center.x - size.x / 2,
            center.y - size.y / 2,
            size.x,
            size.y,
        };
    }

    always_inline constexpr T start() const { return x; }

    always_inline constexpr void start(T value) {
        T d = value - x;
        x += d;
        width -= d;
    }

    always_inline constexpr T end() const { return x + width; }

    always_inline constexpr void end(T value) {
        T d = value - (x + width);
        width += d;
    }

    always_inline constexpr T top() const { return y; }

    always_inline constexpr void top(T value) {
        T d = value - y;
        y += d;
        height -= d;
    }

    always_inline constexpr T bottom() const { return y + height; }

    always_inline constexpr void bottom(T value) {
        T d = value - (y + height);
        height += d;
    }

    always_inline constexpr Vec2<T> topStart() const { return {x, y}; }

    always_inline constexpr Vec2<T> topEnd() const { return {x + width, y}; }

    always_inline constexpr Vec2<T> bottomStart() const { return {x, y + height}; }

    always_inline constexpr Vec2<T> bottomEnd() const { return {x + width, y + height}; }

    always_inline constexpr Vec2<T> center() const { return {x + width / 2, y + height / 2}; }

    always_inline constexpr Vec2<T> topCenter() const { return {x + width / 2, y}; }

    always_inline constexpr Vec2<T> bottomCenter() const { return {x + width / 2, y + height}; }

    always_inline constexpr Vec2<T> startCenter() const { return {x, y + height / 2}; }

    always_inline constexpr Vec2<T> endCenter() const { return {x + width, y + height / 2}; }

    always_inline constexpr Array<Vec2<T>, 4> vertices() const {
        return {
            topStart(),
            topEnd(),
            bottomEnd(),
            bottomStart(),
        };
    }

    always_inline constexpr Vec2<T> size() const { return {width, height}; }

    always_inline constexpr Vec2<T> xw() const { return {x, x + width}; }

    always_inline constexpr void xw(Vec2<T> v) {
        x = v.x;
        width = v.y - v.x;
    }

    always_inline constexpr Vec2<T> yh() const { return {y, y + height}; }

    always_inline constexpr void yh(Vec2<T> v) {
        y = v.x;
        height = v.y - v.x;
    }

    always_inline constexpr bool contains(Vec2<T> v) const {
        return v.x >= x and
               v.y >= y and
               v.x < x + width and
               v.y < y + height;
    }

    always_inline constexpr bool contains(Rect<T> r) const {
        return r.x >= x and
               r.y >= y and
               r.x + r.width <= x + width and
               r.y + r.height <= y + height;
    }

    always_inline constexpr bool colide(Rect<T> r) const {
        return r.x + r.width > x and
               r.y + r.height > y and
               r.x < x + width and
               r.y < y + height;
    }

    always_inline constexpr Rect fit(Rect<T> r) const {
        auto scale = (r.size() / size().template cast<f64>()).min();
        Rect result{0, 0, static_cast<T>(width * scale), static_cast<T>(height * scale)};
        result.xy = r.center() - result.center();
        return result;
    }

    always_inline constexpr Rect cover(Rect<T> r) const {
        f64 scale = (r.size() / size().template cast<f64>()).max();
        Rect result{0, 0, static_cast<T>(width * scale), static_cast<T>(height * scale)};
        result.xy = r.center() - result.center();
        return result;
    }

    always_inline constexpr Rect center(Rect<T> r) const {
        Rect result{0, 0, width, height};
        result.xy = center() - r.center();
        return result;
    }

    always_inline constexpr Rect<T> clipTo(Rect<T> r) const {
        if (not colide(r))
            return {};

        return {
            max(x, r.x),
            max(y, r.y),
            min(x + width, r.x + r.width) - max(x, r.x),
            min(y + height, r.y + r.height) - max(y, r.y)
        };
    }

    always_inline constexpr Rect<T> mergeWith(Rect<T> r) const {
        return fromTwoPoint(
            {
                min(start(), r.start()),
                min(top(), r.top()),
            },
            {
                max(end(), r.end()),
                max(bottom(), r.bottom()),
            }
        );
    }

    always_inline constexpr T operator[](isize i) {
        return _els[i];
    }

    always_inline constexpr T operator[](isize i) const {
        return _els[i];
    }

    template <typename U>
    always_inline constexpr Rect<U> cast() const {
        return {
            static_cast<U>(x),
            static_cast<U>(y),
            static_cast<U>(width),
            static_cast<U>(height),
        };
    }

    always_inline Rect<T> ceil() {
        return fromTwoPoint(
            {
                Math::floor(start()),
                Math::floor(top()),
            },
            {
                Math::ceil(end()),
                Math::ceil(bottom()),
            }
        );
    }

    always_inline Rect<T> floor() {
        return fromTwoPoint(
            {
                Math::ceil(start()),
                Math::ceil(top()),
            },
            {
                Math::floor(end()),
                Math::floor(bottom()),
            }
        );
    }

    always_inline Rect<T> round() {
        return {
            Math::round(x),
            Math::round(y),
            Math::round(width),
            Math::round(height)
        };
    }

    always_inline constexpr bool hasNan() const {
        return xy.hasNan() or wh.hasNan();
    }

    always_inline constexpr Pair<Rect, Rect> hsplit(T v) const {
        return {Rect{x, y, v, height}, Rect{x + v, y, width - v, height}};
    }

    always_inline constexpr Pair<Rect, Rect> vsplit(T v) const {
        return {Rect{x, y, width, v}, Rect{x, y + v, width, height - v}};
    }

    always_inline constexpr Rect shrink(Insets<T> insets) const {
        Rect res = *this;

        res.x += insets.start;
        res.y += insets.top;
        res.width -= insets.start + insets.end;
        res.height -= insets.top + insets.bottom;

        return res;
    }

    always_inline constexpr Math::Rect<T> grow(Insets<T> insets) const {
        Rect res = *this;

        res.x -= insets.start;
        res.y -= insets.top;
        res.width += insets.start + insets.end;
        res.height += insets.top + insets.bottom;

        return res;
    }

    always_inline constexpr Rect offset(Vec2<T> v) const {
        return {x + v.x, y + v.y, width, height};
    }

    always_inline constexpr T area() const {
        return width * height;
    }

    always_inline constexpr T aspect() const {
        return width / height;
    }

    void repr(Io::Emit& e) const {
        e("(rect {} {} {} {})", x, y, width, height);
    }

    constexpr auto map(auto f) const {
        using U = decltype(f(x));
        return Rect<U>{f(x), f(y), f(width), f(height)};
    }
};

template <typename T>
Rect<T> const Rect<T>::ZERO = {};

template <typename T>
Rect<T> const Rect<T>::ONE = {1, 1};

template <typename T>
Rect<T> const Rect<T>::MAX = {Limits<T>::MAX, Limits<T>::MAX};

using Recti = Rect<isize>;

using Rectu = Rect<usize>;

using Rectf = Rect<f64>;

} // namespace Karm::Math
