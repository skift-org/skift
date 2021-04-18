#pragma once

#include <assert.h>
#include <libio/Format.h>
#include <libmath/MinMax.h>
#include <math.h>

namespace Math
{

template <typename Scalar>
class Vec3
{
private:
    Scalar _x;
    Scalar _y;
    Scalar _z;

public:
    static ALWAYS_INLINE Vec3 one() { return Vec3(1, 1, 1); }
    static ALWAYS_INLINE Vec3 oneX() { return Vec3(1, 0, 0); }
    static ALWAYS_INLINE Vec3 oneY() { return Vec3(0, 1, 0); }
    static ALWAYS_INLINE Vec3 oneZ() { return Vec3(0, 0, 1); }
    static ALWAYS_INLINE Vec3 zero() { return Vec3(0, 0, 0); }

    ALWAYS_INLINE const Scalar &x() const { return _x; }
    ALWAYS_INLINE const Scalar &y() const { return _y; }
    ALWAYS_INLINE const Scalar &z() const { return _z; }

    ALWAYS_INLINE Scalar length() const
    {
        return sqrt(_x * _x + _y * _y, _z * _z);
    }

    ALWAYS_INLINE Vec3 extract_x() const { return Vec3(_x, 0, 0); }
    ALWAYS_INLINE Vec3 extract_y() const { return Vec3(0, _y, 0); }
    ALWAYS_INLINE Vec3 extract_z() const { return Vec3(0, 0, _z); }

    ALWAYS_INLINE Vec3() : _x(0), _y(0), _z(0) {}
    ALWAYS_INLINE Vec3(Scalar v) : _x(v), _y(v), _z(v) {}
    ALWAYS_INLINE Vec3(Scalar x, Scalar y, Scalar z) : _x(x), _y(y), _z(z) {}

    template <typename OtherScalar>
    ALWAYS_INLINE Vec3(Vec3<OtherScalar> other)
        : _x(static_cast<Scalar>(other.x())),
          _y(static_cast<Scalar>(other.y())),
          _z(static_cast<Scalar>(other.z()))
    {
    }

    ALWAYS_INLINE Vec3 operator+(const Vec3 &rhs) const
    {
        return Vec3(x() + rhs.x(), y() + rhs.y(), z() + rhs.z());
    }

    ALWAYS_INLINE Vec3 &operator+=(const Vec3 &rhs)
    {
        *this = *this + rhs;
        return *this;
    }

    ALWAYS_INLINE Vec3 operator-() const
    {
        return Vec3(-x(), -y(), -z());
    }

    ALWAYS_INLINE Vec3 operator-(const Vec3 &rhs) const
    {
        return Vec3(x() - rhs.x(), y() - rhs.y(), z() - rhs.z());
    }

    ALWAYS_INLINE Vec3 &operator-=(const Vec3 &rhs)
    {
        *this = *this - rhs;
        return *this;
    }

    ALWAYS_INLINE Vec3 operator*(const Vec3 &rhs) const
    {
        return Vec3(x() * rhs.x(), y() * rhs.y(), z() * rhs.z());
    }

    template <typename U>
    ALWAYS_INLINE Vec3 operator*(const Vec3<U> &rhs) const
    {
        return Vec3(x() * rhs.x(), y() * rhs.y(), z() * rhs.z());
    }

    template <typename U>
    ALWAYS_INLINE Vec3 operator*(U rhs) const
    {
        return Vec3(x() * rhs, y() * rhs, z() * rhs);
    }

    template <typename U>
    ALWAYS_INLINE Vec3 operator/(U rhs) const
    {
        return Vec3(x() / rhs, y() / rhs, z() / rhs);
    }

    ALWAYS_INLINE Vec3 &operator*=(const Vec3 &rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    ALWAYS_INLINE bool operator==(const Vec3 &rhs) const
    {
        return x() == rhs.x() && y() == rhs.y() && z() == rhs.z();
    }

    ALWAYS_INLINE bool operator!=(const Vec3 &rhs) const
    {
        return x() != rhs.x() || y() != rhs.y() || z() != rhs.z();
    }

    ALWAYS_INLINE Scalar distance_to(const Vec3 &destination) const
    {
        return (destination - *this).length();
    }

    ALWAYS_INLINE Vec3 clamped(const Vec3 &min, const Vec3 &max) const
    {
        Scalar xx = clamp(x(), min.x(), max.x());
        Scalar yy = clamp(y(), min.y(), max.y());
        Scalar zz = clamp(z(), min.z(), max.z());

        return {xx, yy, zz};
    }

    ALWAYS_INLINE Vec3 normalized() const
    {
        Scalar magn = length();

        if (magn != 0)
        {
            return *this / length();
        }
        else
        {
            return {0, 0, 0};
        }
    }

    ALWAYS_INLINE Vec3 vector_to(const Vec3 &destination) const
    {
        return (destination - *this).normalized();
    }

    ALWAYS_INLINE Scalar dot(const Vec3 &other) const
    {
        return x() * other.x() + y() * other.y() + z() * other.z();
    }
};

template <typename T>
ResultOr<size_t> format(IO::Writer &writer, const IO::Formating &, const Math::Vec3<T> &vec)
{
    return IO::format(writer, "({}, {}, {})", vec.x(), vec.y(), vec.z());
}

using Vec3i = Vec3<int>;
using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;

} // namespace Math
