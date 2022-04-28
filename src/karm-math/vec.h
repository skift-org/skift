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

    auto min() const -> T {
        return Base::min(x, y);
    }

    auto max() const -> T {
        return Base::max(x, y);
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

    auto min() const -> T {
        return Base::min(x, y, z);
    }

    auto max() const -> T {
        return Base::max(x, y, z);
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

    auto min() const -> T {
        return Base::min(x, y, z, w);
    }

    auto max() const -> T {
        return Base::max(x, y, z, w);
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

} // namespace Karm::Math
