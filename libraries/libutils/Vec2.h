#pragma once

#include <assert.h>
#include <libsystem/math/Dimension.h>
#include <libsystem/math/MinMax.h>
#include <math.h>

template <typename Scalar>
class Vec2
{
private:
    Scalar _x;
    Scalar _y;

public:
    static Vec2 one() { return Vec2(1, 1); }
    static Vec2 oneX() { return Vec2(1, 0); }
    static Vec2 oneY() { return Vec2(0, 1); }
    static Vec2 zero() { return Vec2(0, 0); }

    Scalar x() const { return _x; }
    Scalar y() const { return _y; }

    Scalar length()
    {
        return sqrt(_x * _x + _y * _y);
    }

    Vec2 extract_x() const { return Vec2(_x, 0); }
    Vec2 extract_y() const { return Vec2(0, _y); }

    Vec2() : _x(0), _y(0) {}
    Vec2(Scalar v) : _x(v), _y(v) {}
    Vec2(Scalar x, Scalar y) : _x(x), _y(y) {}

    template <typename OtherScalar>
    Vec2(Vec2<OtherScalar> other)
        : _x(static_cast<Scalar>(other.x())),
          _y(static_cast<Scalar>(other.y()))
    {
    }

    Vec2 operator+(const Vec2 &rhs) const
    {
        return Vec2(x() + rhs.x(), y() + rhs.y());
    }

    Vec2 &operator+=(const Vec2 &rhs)
    {
        *this = *this + rhs;
        return *this;
    }

    Vec2 operator-() const
    {
        return Vec2(-x(), -y());
    }

    Vec2 operator-(const Vec2 &rhs) const
    {
        return Vec2(x() - rhs.x(), y() - rhs.y());
    }

    Vec2 &operator-=(const Vec2 &rhs)
    {
        *this = *this - rhs;
        return *this;
    }

    Vec2 operator*(const Vec2 &rhs) const
    {
        return Vec2(x() * rhs.x(), y() * rhs.y());
    }

    template <typename U>
    Vec2 operator*(const Vec2<U> &rhs) const
    {
        return Vec2(x() * rhs.x(), y() * rhs.y());
    }

    template <typename U>
    Vec2 operator*(U rhs) const
    {
        return Vec2(x() * rhs, y() * rhs);
    }

    template <typename U>
    Vec2 operator/(U rhs) const
    {
        return Vec2(x() / rhs, y() / rhs);
    }

    Vec2 &operator*=(const Vec2 &rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    bool operator==(const Vec2 &rhs) const
    {
        return x() == rhs.x() && y() == rhs.y();
    }

    bool operator!=(const Vec2 &rhs) const
    {
        return x() != rhs.x() || y() != rhs.y();
    }

    double distance_to(const Vec2 &other) const
    {
        return sqrt((x() - other.x()) * (x() - other.x()) +
                    (y() - other.y()) * (y() - other.y()));
    }

    Vec2 clamped(const Vec2 &min, const Vec2 &max) const
    {
        Scalar xx = MAX(min.x(), MIN(max.x(), x()));
        Scalar yy = MAX(min.y(), MIN(max.y(), y()));

        return {xx, yy};
    }

    Vec2 normalized()
    {
        Scalar magn = length();

        if (magn != 0)
        {
            Scalar xx = _x / length();
            Scalar yy = _y / length();

            return {xx, yy};
        }
        else
        {
            return {0, 0};
        }
    }

    Vec2 vector_to(Vec2 destination)
    {
        return (destination - *this).normalized();
    }

    Scalar component(Dimension dim)
    {
        if (dim == Dimension::X)
            return x();

        if (dim == Dimension::Y)
            return y();

        ASSERT_NOT_REACHED();
    }

    Scalar with_component(Dimension dim, Scalar value)
    {
        if (dim == Dimension::X)
            return Vec2i(value, y());

        if (dim == Dimension::Y)
            return Vec2i(x(), value);

        ASSERT_NOT_REACHED();
    }

    Scalar angle_with(Vec2 other)
    {
        auto r = (x() * other.x() + y() * other.y()) / (length() * other.length());

        if (r > 1.0f)
        {
            r = 1.0f;
        }

        if (r < -1.0f)
        {
            r = -1.0f;
        }

        auto sign = (x() * other.y() < y() * other.x()) ? -1.0f : 1.0f;

        return sign * acosf(r);
    }
};

using Vec2i = Vec2<int>;
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;
