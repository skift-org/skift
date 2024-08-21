#pragma once

#include <karm-io/fmt.h>

#include "edge.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
union Trans2 {
    using Scalar = T;

    struct
    {
        T xx;
        T xy;
        T yx;
        T yy;

        T ox;
        T oy;
    };

    struct
    {
        Vec2<T> x;
        Vec2<T> y;
        Vec2<T> o;
    };

    static Trans2 const IDENTITY;

    static constexpr Trans2 makeRotate(T angle) {
        T c = cos(angle);
        T s = sin(angle);
        return {
            c, -s,
            s, c,
            0, 0
        };
    }

    static constexpr Trans2 makeSkew(Vec2<T> v) {
        return {
            1, v.x,
            v.y, 1,
            0, 0
        };
    }

    static constexpr Trans2 makeScale(Vec2<T> v) {
        return {
            v.x, 0,
            0, v.y,
            0, 0
        };
    }

    static constexpr Trans2 makeTranslate(Vec2<T> v) {
        return {
            1, 0,
            0, 1,
            v.x, v.y
        };
    }

    Array<T, 6> _els{};

    bool rotated() const {
        return xx * yy - xy * yx < 0;
    }

    bool skewed() const {
        return xx * yy - xy * yx != 1;
    }

    bool scaled() const {
        return xx != 1 or yy != 1;
    }

    bool translated() const {
        return ox != 0 or oy != 0;
    }

    bool simple() const {
        return not rotated() and not skewed();
    }

    constexpr Trans2()
        : _els{
              1, 0,
              0, 1,
              0, 0
          } {}

    constexpr Trans2(T xx, T xy, T yx, T yy, T ox, T oy)
        : _els{xx, xy, yx, yy, ox, oy} {}

    constexpr Trans2(Trans2 const &other)
        : _els{other._els} {}

    constexpr Trans2(Trans2 &&other)
        : _els{std::move(other._els)} {}

    constexpr Trans2 &operator=(Trans2 const &other) {
        _els = other._els;
        return *this;
    }

    constexpr Trans2 &operator=(Trans2 &&other) {
        _els = std::move(other._els);
        return *this;
    }

    constexpr ~Trans2() {
        _els.~Array();
    }

    constexpr Vec2<T> applyVector(Vec2<T> v) const {
        return {
            v.x * xx + v.y * yx,
            v.x * xy + v.y * yy,
        };
    }

    constexpr Vec2<T> apply(Vec2<T> v) const {
        return applyVector(v) + o;
    }

    constexpr Edge<T> apply(Edge<T> e) const {
        return {apply(e.start), apply(e.end)};
    }

    constexpr Rect<T> apply(Rect<T> r) const {
        return Rect<T>::fromTwoPoint(
            apply(r.topStart()),
            apply(r.bottomEnd())
        );
    }

    constexpr Trans2 multiply(Trans2 const &other) const {
        Trans2 res = {
            xx * other.xx,
            0.0,
            0.0,
            yy * other.yy,
            ox * other.xx + other.ox,
            oy * other.yy + other.oy,
        };

        if (xy != 0.0 or yx != 0.0 ||
            other.xy != 0.0 or other.yx != 0.0) {
            res.xx += xy * other.yx;
            res.xy += xx * other.xy + xy * other.yy;
            res.yx += yx * other.xx + yy * other.yx;
            res.yy += yx * other.xy;
            res.ox += oy * other.yx;
            res.oy += ox * other.xy;
        }

        return res;
    }

    constexpr Trans2 rotated(T angle) {
        return multiply(makeRotate(angle));
    }

    constexpr Trans2 skewed(T x, T y) {
        return multiply(makeSkew(x, y));
    }

    constexpr Trans2 scaled(T x, T y) {
        return multiply(makeScale(x, y));
    }

    constexpr Trans2 scaled(T s) {
        return scaled(s, s);
    }

    constexpr Trans2 translated(T x, T y) {
        return multiply(makeTranslate(x, y));
    }

    constexpr Trans2 inverse() const {
        T det = xx * yy - xy * yx;
        return {
            yy / det, -xy / det,
            -yx / det, xx / det,
            (oy * yy - ox * yx) / det,
            (ox * xy - oy * xx) / det
        };
    }

    constexpr bool isIdentity() const {
        return xx == 1 and xy == 0 and
               yx == 0 and yy == 1 and
               ox == 0 and oy == 0;
    }

    constexpr bool hasNan() const {
        return x.hasNan() or y.hasNan() or o.hasNan();
    }

    constexpr Vec2<T> delta() const {
        return {xx, xy};
    }

    void repr(Io::Emit &e) const {
        e("(trans {} {} {} {} {} {})", xx, xy, yx, yy, ox, oy);
    }
};

template <typename T>
Trans2<T> const Trans2<T>::IDENTITY = {
    1, 0,
    0, 1,
    0, 0
};

using Trans2i = Trans2<isize>;

using Trans2f = Trans2<f64>;

} // namespace Karm::Math
