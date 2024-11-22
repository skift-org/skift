#pragma once

#include "rect.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
union Quad {
    using Scalar = T;

    struct {
        T ax, ay, bx, by, cx, cy, dx, dy;
    };

    struct {
        Vec2<T> a, b, c, d;
    };

    Array<T, 8> _els;

    always_inline constexpr Quad()
        : a(a), b(b), c(c), d(d) {
    }

    always_inline constexpr Quad(Rect<T> r)
        : a(r.topStart()), b(r.topEnd()), c(r.bottomEnd()), d(r.bottomStart()) {
    }

    always_inline constexpr Quad(Vec2<T> a, Vec2<T> b, Vec2<T> c, Vec2<T> d)
        : a(a), b(b), c(c), d(d) {
    }

    always_inline constexpr Quad(T ax, T ay, T bx, T by, T cx, T cy, T dx, T dy)
        : ax(ax), ay(ay), bx(bx), by(by), cx(cx), cy(cy), dx(dx), dy(dy) {
    }

    always_inline constexpr Quad(Quad const &other)
        : _els{other._els} {}

    always_inline constexpr Quad(Quad &&other)
        : _els{std::move(other._els)} {}

    always_inline constexpr Quad &operator=(Quad const &other) {
        _els = other._els;
        return *this;
    }

    always_inline constexpr Quad &operator=(Quad &&other) {
        _els = std::move(other._els);
        return *this;
    }

    always_inline constexpr ~Quad() {
        _els.~Array();
    }

    always_inline constexpr T operator[](isize i) {
        return _els[i];
    }

    always_inline constexpr T operator[](isize i) const {
        return _els[i];
    }

    template <typename U>
    always_inline constexpr Quad<U> cast() const {
        return {
            static_cast<U>(ax),
            static_cast<U>(ay),
            static_cast<U>(bx),
            static_cast<U>(by),
            static_cast<U>(cx),
            static_cast<U>(cy),
            static_cast<U>(dx),
            static_cast<U>(dy),
        };
    }

    always_inline constexpr bool hasNan() const {
        return a.hasNan() or b.hasNan() or c.hasNan() or d.hasNan();
    }

    always_inline Rect<T> bound() const {
        Rect<T> res = {a, 0};
        res = res.mergeWith({b, 0});
        res = res.mergeWith({c, 0});
        res = res.mergeWith({d, 0});
        return res;
    }

    void repr(Io::Emit &e) const {
        e("(quad {} {} {} {})", a, b, c, d);
    }
};

using Quadi = Quad<isize>;

using Quadu = Quad<usize>;

using Quadf = Quad<f64>;

} // namespace Karm::Math
