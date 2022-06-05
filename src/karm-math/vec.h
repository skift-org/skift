#pragma once

#include <karm-base/clamp.h>

namespace Karm::Math {

template <typename T>
union Vec2 {
    struct {
        T x, y;
    };

    struct {
        T width, height;
    };

    T _els[2];

    constexpr Vec2() : _els{0, 0} {}

    constexpr Vec2(T x, T y) : _els{x, y} {}

    constexpr Vec2(T value) : _els{value, value} {}

    T min() const {
        return min(x, y);
    }

    T max() const {
        return max(x, y);
    }

    T dot(Vec2 const &other) const {
        return x * other.x + y * other.y;
    }

    T len() const {
        return sqrt(dot(*this));
    }

    T dist(Vec2 const &other) const {
        return (*this - other).len();
    }

    Vec2 norm() const {
        T len = this->len();
        return {x / len, y / len};
    }

    T angle(Vec2 const &other) const {
        return atan2(other.y - y, other.x - x);
    }

    T operator[](int i) const {
        return _els[i];
    }

    Vec2 operator+(T const &other) const {
        return {x + other, y + other};
    }

    Vec2 operator-(T const &other) const {
        return {x - other, y - other};
    }

    Vec2 operator*(T const &other) const {
        return {x * other, y * other};
    }

    Vec2 operator/(T const &other) const {
        return {x / other, y / other};
    }

    Vec2 operator+(Vec2 const &other) const {
        return {x + other.x, y + other.y};
    }

    Vec2 operator-(Vec2 const &other) const {
        return {x - other.x, y - other.y};
    }

    Vec2 operator*(Vec2 const &other) const {
        return {x * other.x, y * other.y};
    }

    Vec2 operator/(Vec2 const &other) const {
        return {x / other.x, y / other.y};
    }

    template <typename U>
    Vec2<U> cast() const {
        return {static_cast<U>(x), static_cast<U>(y)};
    }
};

using Vec2i = Vec2<int>;

using Vec2f = Vec2<double>;

template <typename T>
union Vec3 {
    struct {
        T x, y, z;
    };

    struct {
        Vec2<T> xy;
        T _z;
    };

    T _els[3];

    constexpr Vec3() : _els{0, 0, 0} {}

    constexpr Vec3(T x, T y, T z) : _els{x, y, z} {}

    constexpr Vec3(T value) : _els{value, value, value} {}

    T min() const {
        return min(x, y, z);
    }

    T max() const {
        return max(x, y, z);
    }

    T dot(Vec3 const &other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    T len() const {
        return sqrt(dot(*this));
    }

    T dist(Vec3 const &other) const {
        return (*this - other).len();
    }

    Vec3 norm() const {
        T len = this->len();
        return {x / len, y / len, z / len};
    }

    T operator[](int i) {
        return _els[i];
    }

    Vec3 operator+(T const &other) const {
        return {x + other, y + other, z + other};
    }

    Vec3 operator-(T const &other) const {
        return {x - other, y - other, z - other};
    }

    Vec3 operator*(T const &other) const {
        return {x * other, y * other, z * other};
    }

    Vec3 operator/(T const &other) const {
        return {x / other, y / other, z / other};
    }

    Vec3 operator+(Vec3 const &other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    Vec3 operator-(Vec3 const &other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    Vec3 operator*(Vec3 const &other) const {
        return {x * other.x, y * other.y, z * other.z};
    }

    Vec3 operator/(Vec3 const &other) const {
        return {x / other.x, y / other.y, z / other.z};
    }
};

using Vec3i = Vec3<int>;

using Vec3f = Vec3<double>;

template <typename T>
union Vec4 {
    struct {
        T x, y, z, w;
    };

    struct {
        Vec3<T> xyz;
        T _w;
    };

    struct {
        Vec2<T> xy;
        Vec2<T> zw;
    };

    T _els[4];

    Vec4() : _els{0, 0, 0, 0} {}

    Vec4(T x, T y, T z, T w) : _els{x, y, z, w} {}

    Vec4(T value) : _els{value, value, value, value} {}

    T min() const {
        return min(x, y, z, w);
    }

    T max() const {
        return max(x, y, z, w);
    }

    T dot(Vec4 const &other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    T len() const {
        return sqrt(dot(*this));
    }

    T dist(Vec4 const &other) const {
        return (*this - other).len();
    }

    Vec4 norm() const {
        T len = this->len();
        return {x / len, y / len, z / len, w / len};
    }

    T operator[](int i) {
        return _els[i];
    }

    Vec4 operator+(T const &other) const {
        return {x + other, y + other, z + other, w + other};
    }

    Vec4 operator-(T const &other) const {
        return {x - other, y - other, z - other, w - other};
    }

    Vec4 operator*(T const &other) const {
        return {x * other, y * other, z * other, w * other};
    }

    Vec4 operator/(T const &other) const {
        return {x / other, y / other, z / other, w / other};
    }

    Vec4 operator+(Vec4 const &other) const {
        return {x + other.x, y + other.y, z + other.z, w + other.w};
    }

    Vec4 operator-(Vec4 const &other) const {
        return {x - other.x, y - other.y, z - other.z, w - other.w};
    }

    Vec4 operator*(Vec4 const &other) const {
        return {x * other.x, y * other.y, z * other.z, w * other.w};
    }

    Vec4 operator/(Vec4 const &other) const {
        return {x / other.x, y / other.y, z / other.z, w / other.w};
    }
};

using Vec4i = Vec4<int>;

using Vec4f = Vec4<double>;

} // namespace Karm::Math
