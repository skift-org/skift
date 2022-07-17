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

    constexpr T min() const {
        return min(x, y);
    }

    constexpr T max() const {
        return max(x, y);
    }

    constexpr T dot(Vec2 const &other) const {
        return x * other.x + y * other.y;
    }

    constexpr T len() const {
        return sqrt(dot(*this));
    }

    constexpr T dist(Vec2 const &other) const {
        return (*this - other).len();
    }

    constexpr Vec2 norm() const {
        T len = this->len();
        return {x / len, y / len};
    }

    constexpr T angle(Vec2 const &other) const {
        return atan2(other.y - y, other.x - x);
    }

    constexpr T operator[](int i) const {
        return _els[i];
    }

    constexpr Vec2 operator+(T const &other) const {
        return {x + other, y + other};
    }

    constexpr Vec2 operator-(T const &other) const {
        return {x - other, y - other};
    }

    constexpr Vec2 operator*(T const &other) const {
        return {x * other, y * other};
    }

    constexpr Vec2 operator/(T const &other) const {
        return {x / other, y / other};
    }

    constexpr Vec2 operator+(Vec2 const &other) const {
        return {x + other.x, y + other.y};
    }

    constexpr Vec2 operator-(Vec2 const &other) const {
        return {x - other.x, y - other.y};
    }

    constexpr Vec2 operator*(Vec2 const &other) const {
        return {x * other.x, y * other.y};
    }

    constexpr Vec2 operator/(Vec2 const &other) const {
        return {x / other.x, y / other.y};
    }

    template <typename U>
    constexpr Vec2<U> cast() const {
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

    constexpr T min() const {
        return min(x, y, z);
    }

    constexpr T max() const {
        return max(x, y, z);
    }

    constexpr T dot(Vec3 const &other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    constexpr T len() const {
        return sqrt(dot(*this));
    }

    constexpr T dist(Vec3 const &other) const {
        return (*this - other).len();
    }

    constexpr Vec3 norm() const {
        T len = this->len();
        return {x / len, y / len, z / len};
    }

    constexpr T operator[](int i) {
        return _els[i];
    }

    constexpr Vec3 operator+(T const &other) const {
        return {x + other, y + other, z + other};
    }

    constexpr Vec3 operator-(T const &other) const {
        return {x - other, y - other, z - other};
    }

    constexpr Vec3 operator*(T const &other) const {
        return {x * other, y * other, z * other};
    }

    constexpr Vec3 operator/(T const &other) const {
        return {x / other, y / other, z / other};
    }

    constexpr Vec3 operator+(Vec3 const &other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    constexpr Vec3 operator-(Vec3 const &other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    constexpr Vec3 operator*(Vec3 const &other) const {
        return {x * other.x, y * other.y, z * other.z};
    }

    constexpr Vec3 operator/(Vec3 const &other) const {
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

    T _els[4]{};

    constexpr Vec4() = default;

    constexpr Vec4(T x, T y, T z, T w) : _els{x, y, z, w} {}

    constexpr Vec4(T value) : _els{value, value, value, value} {}

    constexpr T min() const {
        return min(x, y, z, w);
    }

    constexpr T max() const {
        return max(x, y, z, w);
    }

    constexpr T dot(Vec4 const &other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    constexpr T len() const {
        return sqrt(dot(*this));
    }

    constexpr T dist(Vec4 const &other) const {
        return (*this - other).len();
    }

    constexpr Vec4 norm() const {
        T len = this->len();
        return {x / len, y / len, z / len, w / len};
    }

    constexpr T operator[](int i) {
        return _els[i];
    }

    constexpr Vec4 operator+(T const &other) const {
        return {x + other, y + other, z + other, w + other};
    }

    constexpr Vec4 operator-(T const &other) const {
        return {x - other, y - other, z - other, w - other};
    }

    constexpr Vec4 operator*(T const &other) const {
        return {x * other, y * other, z * other, w * other};
    }

    constexpr Vec4 operator/(T const &other) const {
        return {x / other, y / other, z / other, w / other};
    }

    constexpr Vec4 operator+(Vec4 const &other) const {
        return {x + other.x, y + other.y, z + other.z, w + other.w};
    }

    constexpr Vec4 operator-(Vec4 const &other) const {
        return {x - other.x, y - other.y, z - other.z, w - other.w};
    }

    constexpr Vec4 operator*(Vec4 const &other) const {
        return {x * other.x, y * other.y, z * other.z, w * other.w};
    }

    constexpr Vec4 operator/(Vec4 const &other) const {
        return {x / other.x, y / other.y, z / other.z, w / other.w};
    }
};

using Vec4i = Vec4<int>;

using Vec4f = Vec4<double>;

} // namespace Karm::Math
