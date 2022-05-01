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

    double _els[6];

    Trans2() : _els{1, 0, 0, 1, 0, 0} {}

    Trans2(double xx, double xy, double yx, double yy, double ox, double oy) : _els{xx, xy, yx, yy, ox, oy} {}

    static auto rotate(double angle) -> Trans2 {
        double c = cos(angle);
        double s = sin(angle);
        return {c, -s, s, c, 0, 0};
    }

    static auto skew(double x, double y) -> Trans2 {
        return {1, x, y, 1, 0, 0};
    }

    static auto scale(double x, double y) -> Trans2 {
        return {x, 0, 0, y, 0, 0};
    }

    static auto translate(double x, double y) -> Trans2 {
        return {1, 0, 0, 1, x, y};
    }

    auto apply(Vec2f const &v) const -> Vec2f {
        return {
            v.x * xx + v.y * xy,
            v.x * yx + v.y * yy,
        };
    }

    auto operator*(Trans2 const &other) const -> Trans2 {
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
