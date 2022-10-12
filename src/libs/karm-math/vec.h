#pragma once

#include <karm-base/clamp.h>
#include <karm-fmt/fmt.h>

#include "funcs.h"

namespace Karm::Math {

template <typename T>
union Vec2 {
    using Scalar = T;

    struct {
        T x, y;
    };

    struct {
        T width, height;
    };

    T _els[2];

    constexpr Vec2() : _els{} {}

    constexpr Vec2(T x, T y) : _els{x, y} {}

    constexpr Vec2(T value) : _els{value, value} {}

    constexpr T min() const {
        return Karm::min(x, y);
    }

    constexpr T max() const {
        return Karm::max(x, y);
    }

    constexpr Vec2 min(Vec2 const &other) const {
        return {Karm::min(x, other.x), Karm::min(y, other.y)};
    }

    constexpr Vec2 max(Vec2 const &other) const {
        return {Karm::max(x, other.x), Karm::max(y, other.y)};
    }

    constexpr T dot(Vec2 const &other) const {
        return x * other.x + y * other.y;
    }

    constexpr T cross(Vec2 const &other) const {
        return x * other.y - y * other.x;
    }

    constexpr T len() const {
        return sqrt(dot(*this));
    }

    constexpr T lenSq() const {
        return dot(*this);
    }

    constexpr T dist(Vec2 const &other) const {
        return (*this - other).len();
    }

    constexpr Vec2 norm() const {
        T len = this->len();
        return Vec2{x, y} / len;
    }

    constexpr T angle() const {
        return atan2(y, x);
    }

    constexpr T angleWith(Vec2 const &other) const {
        auto r = norm().dot(other.norm());
        auto sign = (x * other.y < y * other.x) ? -1.0 : 1.0;
        return sign * acos(r);
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

    template <typename U>
    constexpr auto operator+(Vec2<U> const &other) const -> Vec2<decltype(T{} + U{})> {
        return {x + other.x, y + other.y};
    }

    template <typename U>
    constexpr auto operator-(Vec2<U> const &other) const -> Vec2<decltype(T{} - U{})> {
        return {x - other.x, y - other.y};
    }

    template <typename U>
    constexpr auto operator*(Vec2<U> const &other) const -> Vec2<decltype(T{} * U{})> {
        return {x * other.x, y * other.y};
    }

    template <typename U>
    constexpr auto operator/(Vec2<U> const &other) const -> Vec2<decltype(T{} / U{})> {
        return {x / other.x, y / other.y};
    }

    constexpr Vec2 operator-() const {
        return {-x, -y};
    }

    template <typename U>
    constexpr Vec2<U> cast() const {
        return {static_cast<U>(x), static_cast<U>(y)};
    }

    bool hasNan() const {
        return std::isnan(x) || std::isnan(y);
    }
};

template <typename T>
Vec2<T> operator+(T const &lhs, Vec2<T> const &rhs) {
    return rhs + lhs;
}

template <typename T>
Vec2<T> operator-(T const &lhs, Vec2<T> const &rhs) {
    return rhs - lhs;
}

template <typename T>
Vec2<T> operator*(T const &lhs, Vec2<T> const &rhs) {
    return rhs * lhs;
}

template <typename T>
Vec2<T> operator/(T const &lhs, Vec2<T> const &rhs) {
    return rhs / lhs;
}

using Vec2i = Vec2<int>;

using Vec2u = Vec2<unsigned int>;

using Vec2f = Vec2<double>;

template <typename T>
bool epsilonEq(Vec2<T> const &lhs, Vec2<T> const &rhs, T epsilon) {
    return epsilonEq(lhs.x, rhs.x, epsilon) &&
           epsilonEq(lhs.y, rhs.y, epsilon);
}

template <typename T>
union Vec3 {
    using Scalar = T;

    struct {
        T x, y, z;
    };

    struct {
        Vec2<T> xy;
        T _z;
    };

    struct {
        T _x;
        Vec2<T> yz;
    };

    T _els[3];

    constexpr Vec3() : _els{} {}

    constexpr Vec3(T x, T y, T z) : _els{x, y, z} {}

    constexpr Vec3(T value) : _els{value, value, value} {}

    constexpr Vec2<T> xz() const {
        return {x, y};
    }

    constexpr T min() const {
        return Karm::min(x, y, z);
    }

    constexpr T max() const {
        return Karm::max(x, y, z);
    }

    constexpr Vec3 min(Vec3 const &other) {
        return {Karm::min(x, other.x), Karm::min(y, other.y), Karm::min(z, other.z)};
    }

    constexpr Vec3 max(Vec3<T> const &other) {
        return {Karm::max(x, other.x), Karm::max(y, other.y), Karm::max(z, other.z)};
    }

    constexpr T dot(Vec3 const &other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    constexpr T lenSq() const {
        return dot(*this);
    }

    constexpr T len() const {
        return sqrt(lenSq());
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

    constexpr Vec3 operator-() const {
        return {-x, -y, -z};
    }

    bool hasNan() const {
        return std::isnan(x) || std::isnan(y) || std::isnan(z);
    }

    template <typename U>
    constexpr Vec3<U> cast() const {
        return {static_cast<U>(x), static_cast<U>(y), static_cast<U>(z)};
    }
};

template <typename T>
constexpr Vec3<T> operator+(T const &lhs, Vec3<T> const &rhs) {
    return {lhs + rhs.x, lhs + rhs.y, lhs + rhs.z};
}

template <typename T>
constexpr Vec3<T> operator-(T const &lhs, Vec3<T> const &rhs) {
    return {lhs - rhs.x, lhs - rhs.y, lhs - rhs.z};
}

template <typename T>
constexpr Vec3<T> operator*(T const &lhs, Vec3<T> const &rhs) {
    return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z};
}

template <typename T>
constexpr Vec3<T> operator/(T const &lhs, Vec3<T> const &rhs) {
    return {lhs / rhs.x, lhs / rhs.y, lhs / rhs.z};
}

using Vec3i = Vec3<int>;

using Vec3u = Vec3<unsigned int>;

using Vec3f = Vec3<double>;

template <typename T>
bool epsilonEq(Vec3<T> const &lhs, Vec3<T> const &rhs, T epsilon) {
    return epsilonEq(lhs.x, rhs.x, epsilon) &&
           epsilonEq(lhs.y, rhs.y, epsilon) &&
           epsilonEq(lhs.z, rhs.z, epsilon);
}

template <typename T>
union Vec4 {
    using Scalar = T;

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

    constexpr Vec4() : _els{} {}

    constexpr Vec4(T x, T y, T z, T w) : _els{x, y, z, w} {}

    constexpr Vec4(T value) : _els{value, value, value, value} {}

    constexpr T min() const {
        return min(x, y, z, w);
    }

    constexpr T max() const {
        return max(x, y, z, w);
    }

    constexpr Vec4 min(Vec4 const &other) {
        return {min(x, other.x), min(y, other.y), min(z, other.z), min(w, other.w)};
    }

    constexpr Vec4 max(Vec4 const &other) {
        return {max(x, other.x), max(y, other.y), max(z, other.z), max(w, other.w)};
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

    constexpr Vec4 operator-() const {
        return {-x, -y, -z, -w};
    }

    bool hasNan() const {
        return std::isnan(x) || std::isnan(y) || std::isnan(z) || std::isnan(w);
    }
};

template <typename T>
constexpr Vec4<T> operator+(T const &lhs, Vec4<T> const &rhs) {
    return {lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w};
}

template <typename T>
constexpr Vec4<T> operator-(T const &lhs, Vec4<T> const &rhs) {
    return {lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w};
}

template <typename T>
constexpr Vec4<T> operator*(T const &lhs, Vec4<T> const &rhs) {
    return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w};
}

template <typename T>
constexpr Vec4<T> operator/(T const &lhs, Vec4<T> const &rhs) {
    return {lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w};
}

using Vec4i = Vec4<int>;

using Vec4u = Vec4<unsigned int>;

using Vec4f = Vec4<double>;

template <typename T>
bool epsilonEq(Vec4<T> const &lhs, Vec4<T> const &rhs, T epsilon) {
    return epsilonEq(lhs.x, rhs.x, epsilon) &&
           epsilonEq(lhs.y, rhs.y, epsilon) &&
           epsilonEq(lhs.z, rhs.z, epsilon) &&
           epsilonEq(lhs.w, rhs.w, epsilon);
}

} // namespace Karm::Math

template <typename T>
struct Karm::Fmt::Formatter<Math::Vec2<T>> {
    Result<size_t> format(Io::_TextWriter &writer, Math::Vec2<T> vec) {
        return Fmt::format(writer, "Vec2({}, {})", vec.x, vec.y);
    }
};

template <typename T>
struct Karm::Fmt::Formatter<Math::Vec3<T>> {
    Result<size_t> format(Io::_TextWriter &writer, Math::Vec3<T> vec) {
        return Fmt::format(writer, "Vec3({}, {}, {})", vec.x, vec.y, vec.z);
    }
};

template <typename T>
struct Karm::Fmt::Formatter<Math::Vec4<T>> {
    Result<size_t> format(Io::_TextWriter &writer, Math::Vec4<T> vec) {
        return Fmt::format(writer, "Vec4({}, {}, {}, {})", vec.x, vec.y, vec.z);
    }
};
