#pragma once

#include <libutils/Vec2.h>

#include <math.h>
#include <string.h>

template <typename Scalar>
class Trans2
{
private:
    Scalar _m[6] = {};

public:
    static Trans2 identity()
    {
        return {
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            0.0f,
            0.0f,
        };
    }

    static Trans2 translation(Vec2<Scalar> vec)
    {
        return {
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            vec.x(),
            vec.y(),
        };
    }

    static Trans2 scale(Scalar s) { return scale({s, s}); }

    static Trans2 scale(Vec2<Scalar> s)
    {
        return {
            s.x(),
            0.0f,
            0.0f,
            s.y(),
            0,
            0,
        };
    }

    static Trans2 skewX(Scalar a)
    {
        return {
            1.0f,
            0.0f,
            tanf(a),
            1.0f,
            0.0f,
            0.0f,
        };
    }

    static Trans2 skewY(Scalar a)
    {
        return {
            1.0f,
            tanf(a),
            0.0f,
            1.0f,
            0.0f,
            0.0f,
        };
    }

    static Trans2 rotation(Scalar a)
    {
        Scalar cs = cosf(a);
        Scalar sn = sinf(a);

        return {
            cs,
            sn,
            -sn,
            cs,
            0.0f,
            0.0f,
        };
    }

    Scalar operator[](size_t i) const
    {
        assert(i < 6);
        return _m[i];
    }

    Trans2()
    {
    }

    Trans2(const Scalar m[6])
    {
        memcpy(_m, m, sizeof(Scalar) * 6);
    }

    Trans2(Scalar m0, Scalar m1, Scalar m2, Scalar m3, Scalar m4, Scalar m5)
        : _m{
              m0,
              m1,
              m2,
              m3,
              m4,
              m5,
          }
    {
    }

    Scalar determinant() const
    {
        return _m[0] * _m[3] - _m[2] * _m[1];
    }

    Trans2 inverse() const
    {
        auto det = determinant();

        if (det > -1e-6 && det < 1e-6)
        {
            return identity();
        }

        auto invdet = 1 / det;

        return {
            _m[3] * invdet,
            -_m[2] * invdet,
            (_m[2] * _m[5] - _m[3] * _m[4]) * invdet,
            -_m[1] * invdet,
            _m[0] * invdet,
            (_m[1] * _m[4] - _m[0] * _m[5]) * invdet,
        };
    }

    Vec2<Scalar> apply(const Vec2<Scalar> v) const
    {
        return {
            v.x() * _m[0] + v.y() * _m[2] + _m[4],
            v.x() * _m[1] + v.y() * _m[3] + _m[5],
        };
    }

    Vec2<Scalar> apply_no_translation(const Vec2<Scalar> v) const
    {
        return {
            v.x() * _m[0] + v.y() * _m[2],
            v.x() * _m[1] + v.y() * _m[3],
        };
    }

    Trans2 operator*(const Trans2 other) const
    {
        return {
            _m[0] * other[0] + _m[1] * other[2],
            _m[0] * other[1] + _m[1] * other[3],
            _m[2] * other[0] + _m[3] * other[2],
            _m[2] * other[1] + _m[3] * other[3],
            _m[4] * other[1] + _m[5] * other[3] + other[5],
            _m[4] * other[0] + _m[5] * other[2] + other[4],
        };
    }
};

using Trans2f = Trans2<float>;
using Trans2d = Trans2<double>;
