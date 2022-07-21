#pragma once

#include "vec.h"

namespace Karm::Math {

union Trans2 {
    struct
    {
        double xx;
        double xy;
        double yx;
        double yy;

        double ox;
        double oy;
    };

    struct
    {
        Vec2<double> x;
        Vec2<double> y;
        Vec2<double> o;
    };

    double _els[6]{};

    constexpr Trans2() : _els{1, 0, 0, 1, 0, 0} {}

    constexpr Trans2(double xx, double xy, double yx, double yy, double ox, double oy) : _els{xx, xy, yx, yy, ox, oy} {}

    static constexpr Trans2 rotate(double angle) {
        double c = cos(angle);
        double s = sin(angle);
        return {c, -s, s, c, 0, 0};
    }

    static constexpr Trans2 skew(double x, double y) {
        return {1, x, y, 1, 0, 0};
    }

    static constexpr Trans2 scale(double x, double y) {
        return {x, 0, 0, y, 0, 0};
    }

    static constexpr Trans2 translate(double x, double y) {
        return {1, 0, 0, 1, x, y};
    }

    constexpr Vec2f apply(Vec2f v) const {
        return {
            v.x * xx + v.y * xy,
            v.x * yx + v.y * yy,
        };
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
};

} // namespace Karm::Math
