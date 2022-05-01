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

    auto min() const -> T {
        return Base::min(x, y);
    }

    auto max() const -> T {
        return Base::max(x, y);
    }

    auto dot(Vec2 const &other) const -> T {
        return x * other.x + y * other.y;
    }

    auto len() const -> T {
        return sqrt(dot(*this));
    }

    auto dist(Vec2 const &other) const -> T {
        return (*this - other).len();
    }

    auto norm() const -> Vec2 {
        T len = this->len();
        return {x / len, y / len};
    }

    auto angle(Vec2 const &other) const -> T {
        return atan2(other.y - y, other.x - x);
    }

    auto operator[](int i) const -> T {
        return _els[i];
    }

    auto operator+(T const &other) const -> Vec2 {
        return {x + other, y + other};
    }

    auto operator-(T const &other) const -> Vec2 {
        return {x - other, y - other};
    }

    auto operator*(T const &other) const -> Vec2 {
        return {x * other, y * other};
    }

    auto operator/(T const &other) const -> Vec2 {
        return {x / other, y / other};
    }

    auto operator+(Vec2 const &other) const -> Vec2 {
        return {x + other.x, y + other.y};
    }

    auto operator-(Vec2 const &other) const -> Vec2 {
        return {x - other.x, y - other.y};
    }

    auto operator*(Vec2 const &other) const -> Vec2 {
        return {x * other.x, y * other.y};
    }

    auto operator/(Vec2 const &other) const -> Vec2 {
        return {x / other.x, y / other.y};
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

    auto min() const -> T {
        return Base::min(x, y, z);
    }

    auto max() const -> T {
        return Base::max(x, y, z);
    }

    auto dot(Vec3 const &other) const -> T {
        return x * other.x + y * other.y + z * other.z;
    }

    auto len() const -> T {
        return sqrt(dot(*this));
    }

    auto dist(Vec3 const &other) const -> T {
        return (*this - other).len();
    }

    auto norm() const -> Vec3 {
        T len = this->len();
        return {x / len, y / len, z / len};
    }

    auto operator[](int i) -> T {
        return _els[i];
    }

    auto operator+(T const &other) const -> Vec3 {
        return {x + other, y + other, z + other};
    }

    auto operator-(T const &other) const -> Vec3 {
        return {x - other, y - other, z - other};
    }

    auto operator*(T const &other) const -> Vec3 {
        return {x * other, y * other, z * other};
    }

    auto operator/(T const &other) const -> Vec3 {
        return {x / other, y / other, z / other};
    }

    auto operator+(Vec3 const &other) const -> Vec3 {
        return {x + other.x, y + other.y, z + other.z};
    }

    auto operator-(Vec3 const &other) const -> Vec3 {
        return {x - other.x, y - other.y, z - other.z};
    }

    auto operator*(Vec3 const &other) const -> Vec3 {
        return {x * other.x, y * other.y, z * other.z};
    }

    auto operator/(Vec3 const &other) const -> Vec3 {
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

    auto min() const -> T {
        return Base::min(x, y, z, w);
    }

    auto max() const -> T {
        return Base::max(x, y, z, w);
    }

    auto dot(Vec4 const &other) const -> T {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    auto len() const -> T {
        return sqrt(dot(*this));
    }

    auto dist(Vec4 const &other) const -> T {
        return (*this - other).len();
    }

    auto norm() const -> Vec4 {
        T len = this->len();
        return {x / len, y / len, z / len, w / len};
    }

    auto operator[](int i) -> T {
        return _els[i];
    }

    auto operator+(T const &other) const -> Vec4 {
        return {x + other, y + other, z + other, w + other};
    }

    auto operator-(T const &other) const -> Vec4 {
        return {x - other, y - other, z - other, w - other};
    }

    auto operator*(T const &other) const -> Vec4 {
        return {x * other, y * other, z * other, w * other};
    }

    auto operator/(T const &other) const -> Vec4 {
        return {x / other, y / other, z / other, w / other};
    }

    auto operator+(Vec4 const &other) const -> Vec4 {
        return {x + other.x, y + other.y, z + other.z, w + other.w};
    }

    auto operator-(Vec4 const &other) const -> Vec4 {
        return {x - other.x, y - other.y, z - other.z, w - other.w};
    }

    auto operator*(Vec4 const &other) const -> Vec4 {
        return {x * other.x, y * other.y, z * other.z, w * other.w};
    }

    auto operator/(Vec4 const &other) const -> Vec4 {
        return {x / other.x, y / other.y, z / other.z, w / other.w};
    }
};

using Vec4i = Vec4<int>;

using Vec4f = Vec4<double>;

} // namespace Karm::Math
