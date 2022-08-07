#pragma once

#include <karm-fmt/fmt.h>

#include "vec.h"

namespace Karm::Math {

template <typename T>
union Trans2 {
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

    T _els[6]{};

    constexpr Trans2() : _els{1, 0, 0, 1, 0, 0} {}

    constexpr Trans2(T xx, T xy, T yx, T yy, T ox, T oy) : _els{xx, xy, yx, yy, ox, oy} {}

    static constexpr Trans2 rotate(T angle) {
        T c = cos(angle);
        T s = sin(angle);
        return {c, -s, s, c, 0, 0};
    }

    static constexpr Trans2 skew(T x, T y) {
        return {1, x, y, 1, 0, 0};
    }

    static constexpr Trans2 scale(T x, T y) {
        return {x, 0, 0, y, 0, 0};
    }

    static constexpr Trans2 translate(T x, T y) {
        return {1, 0, 0, 1, x, y};
    }

    constexpr Vec2<T> applyVector(Vec2<T> v) const {
        return {
            v.x * xx + v.y * yx,
            v.x * xy + v.y * yy,
        };
    }

    constexpr Vec2<T> applyPoint(Vec2<T> v) const {
        return applyVector(v) + o;
    }

    constexpr Trans2 operator*(Trans2 const &other) const {
        return {
            xx * other.xx + xy * other.yx,
            xx * other.xy + xy * other.yy,
            yx * other.xx + yy * other.yx,
            yx * other.xy + yy * other.yy,
            ox * other.xx + oy * other.yx + other.ox,
            ox * other.xy + oy * other.yy + other.oy,
        };
    }

    bool hasNan() const {
        return x.hasNan() || y.hasNan() || o.hasNan();
    }
};

using Trans2i = Trans2<int>;

using Trans2f = Trans2<double>;

} // namespace Karm::Math

template <typename T>
struct Karm::Fmt::Formatter<Math::Trans2<T>> {
    Result<size_t> format(Io::_TextWriter &writer, Math::Trans2<T> trans) {
        return Fmt::format(writer, "Trans2({}, {}, {}, {}, {}, {})", trans.xx, trans.xy, trans.yx, trans.yy, trans.ox, trans.oy);
    }
};
