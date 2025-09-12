#pragma once

import Karm.Core;

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

    static Vec2 const ZERO;

    static Vec2 const ONE;

    static Vec2 const MAX;

    Array<T, 2> _els;

    constexpr Vec2()
        : _els{} {}

    constexpr Vec2(T x, T y)
        : _els{x, y} {}

    constexpr Vec2(T value)
        : _els{value, value} {}

    constexpr Vec2(Vec2 const& other)
        : _els{other._els} {}

    constexpr Vec2(Vec2&& other)
        : _els{std::move(other._els)} {}

    constexpr Vec2& operator=(Vec2 const& other) {
        _els = other._els;
        return *this;
    }

    constexpr Vec2& operator=(Vec2&& other) {
        _els = std::move(other._els);
        return *this;
    }

    constexpr ~Vec2() {
        _els.~Array();
    }

    constexpr T min() const {
        return Karm::min(x, y);
    }

    constexpr T max() const {
        return Karm::max(x, y);
    }

    constexpr Vec2 min(Vec2 other) const {
        return {Karm::min(x, other.x), Karm::min(y, other.y)};
    }

    constexpr Vec2 max(Vec2 other) const {
        return {Karm::max(x, other.x), Karm::max(y, other.y)};
    }

    constexpr T dot(Vec2 other) const {
        return x * other.x + y * other.y;
    }

    constexpr T cross(Vec2 other) const {
        return x * other.y - y * other.x;
    }

    constexpr T len() const {
        return sqrt(dot(*this));
    }

    constexpr T lenSq() const {
        return dot(*this);
    }

    constexpr T dist(Vec2 other) const {
        return (*this - other).len();
    }

    constexpr Vec2 unit() const {
        return Vec2{x, y} / len();
    }

    constexpr T angle() const {
        return atan2(y, x);
    }

    constexpr Vec2 rotate(T angle) const {
        T c = cos(angle);
        T s = sin(angle);
        return {x * c - y * s, x * s + y * c};
    }

    constexpr Vec2 normal() const {
        return {-y, x};
    }

    constexpr T angleWith(Vec2 other) const {
        auto r = unit().dot(other.unit());
        auto sign = (x * other.y < y * other.x) ? -1.0 : 1.0;
        return sign * acos(r);
    }

    constexpr T operator[](isize i) const {
        return _els[i];
    }

    constexpr Vec2 operator+(T const& other) const {
        return {x + other, y + other};
    }

    constexpr Vec2 operator-(T const& other) const {
        return {x - other, y - other};
    }

    constexpr Vec2 operator*(T const& other) const {
        return {x * other, y * other};
    }

    constexpr Vec2 operator/(T const& other) const {
        return {x / other, y / other};
    }

    template <typename U>
    constexpr auto operator+(Vec2<U> const& other) const
        -> Vec2<decltype(T{} + U{})> {
        return {x + other.x, y + other.y};
    }

    template <typename U>
    constexpr auto operator-(Vec2<U> const& other) const
        -> Vec2<decltype(T{} - U{})> {
        return {x - other.x, y - other.y};
    }

    template <typename U>
    constexpr auto operator*(Vec2<U> const& other) const
        -> Vec2<decltype(T{} * U{})> {
        return {x * other.x, y * other.y};
    }

    template <typename U>
    constexpr auto operator/(Vec2<U> const& other) const
        -> Vec2<decltype(T{} / U{})> {
        return {x / other.x, y / other.y};
    }

    constexpr Vec2 operator-() const {
        return {-x, -y};
    }

    template <typename U>
    constexpr Vec2<U> cast() const {
        return {static_cast<U>(x), static_cast<U>(y)};
    }

    template <typename U>
    constexpr U into() const {
        return U{x, y};
    }

    Vec2 floor() {
        return {Math::floor(x), Math::floor(y)};
    }

    Vec2 ceil() {
        return {Math::ceil(x), Math::ceil(y)};
    }

    Vec2 round() {
        return {Math::round(x), Math::round(y)};
    }

    bool hasNan() const {
        return isNan(x) or isNan(y);
    }

    auto operator<=>(Vec2 const& other) const {
        return _els <=> other._els;
    }

    bool operator==(Vec2 const& other) const {
        return _els == other._els;
    }

    void repr(Io::Emit& e) const {
        e("(vec {} {})", x, y);
    }

    constexpr auto map(auto f) const {
        using U = decltype(f(x));
        return Vec2<U>{f(x), f(y)};
    }
};

template <typename T>
Vec2<T> operator+(T const& lhs, Vec2<T> const& rhs) {
    return rhs + lhs;
}

template <typename T>
Vec2<T> operator-(T const& lhs, Vec2<T> const& rhs) {
    return rhs - lhs;
}

template <typename T>
Vec2<T> operator*(T const& lhs, Vec2<T> const& rhs) {
    return rhs * lhs;
}

template <typename T>
Vec2<T> operator/(T const& lhs, Vec2<T> const& rhs) {
    return rhs / lhs;
}

template <typename T>
constexpr Vec2<T> Vec2<T>::ZERO = {};

template <typename T>
constexpr Vec2<T> Vec2<T>::ONE = {1};

template <typename T>
constexpr Vec2<T> Vec2<T>::MAX = {Limits<T>::MAX};

using Vec2i = Vec2<isize>;

using Vec2u = Vec2<usize>;

using Vec2f = Vec2<f64>;

template <typename T>
bool epsilonEq(Vec2<T> const& lhs, Vec2<T> const& rhs, T epsilon = Limits<T>::EPSILON) {
    return epsilonEq(lhs.x, rhs.x, epsilon) and
           epsilonEq(lhs.y, rhs.y, epsilon);
}

template <typename T>
union Vec3 {
    using Scalar = T;

    struct {
        T x, y, z;
    };

    struct {
        T r, g, b;
    };

    struct {

        Vec2<T> xy;
        T _z;
    };

    struct {
        T _x;
        Vec2<T> yz;
    };

    Array<T, 3> _els;

    constexpr Vec3()
        : _els{} {}

    constexpr Vec3(T x, T y, T z)
        : _els{x, y, z} {}

    constexpr Vec3(T value)
        : _els{value, value, value} {}

    constexpr Vec3(Vec3 const& other)
        : _els{other._els} {}

    constexpr Vec3(Vec3&& other)
        : _els{std::move(other._els)} {}

    constexpr Vec3& operator=(Vec3 const& other) {
        _els = other._els;
        return *this;
    }

    constexpr Vec3& operator=(Vec3&& other) {
        _els = std::move(other._els);
        return *this;
    }

    constexpr ~Vec3() {
        _els.~Array();
    }

    constexpr Vec2<T> xz() const {
        return {x, y};
    }

    constexpr T min() const {
        return Karm::min(x, y, z);
    }

    constexpr T max() const {
        return Karm::max(x, y, z);
    }

    constexpr Vec3 min(Vec3 const& other) {
        return {
            Karm::min(x, other.x),
            Karm::min(y, other.y),
            Karm::min(z, other.z),
        };
    }

    constexpr Vec3 max(Vec3<T> const& other) {
        return {
            Karm::max(x, other.x),
            Karm::max(y, other.y),
            Karm::max(z, other.z),
        };
    }

    constexpr T dot(Vec3 const& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    constexpr T lenSq() const {
        return dot(*this);
    }

    constexpr T len() const {
        return sqrt(lenSq());
    }

    constexpr T dist(Vec3 const& other) const {
        return (*this - other).len();
    }

    constexpr Vec3 unit() const {
        T l = len();
        return {x / l, y / l, z / l};
    }

    constexpr T operator[](isize i) {
        return _els[i];
    }

    constexpr Vec3 operator+(T const& other) const {
        return {x + other, y + other, z + other};
    }

    constexpr Vec3 operator-(T const& other) const {
        return {x - other, y - other, z - other};
    }

    constexpr Vec3 operator*(T const& other) const {
        return {x * other, y * other, z * other};
    }

    constexpr Vec3 operator/(T const& other) const {
        return {x / other, y / other, z / other};
    }

    template <typename U>
    constexpr auto operator+(Vec3<U> const& other) const
        -> Vec3<decltype(T{} + U{})> {
        return {x + other.x, y + other.y, z + other.z};
    }

    template <typename U>
    constexpr auto operator-(Vec3<U> const& other) const
        -> Vec3<decltype(T{} - U{})> {
        return {x - other.x, y - other.y, z - other.z};
    }

    template <typename U>
    constexpr auto operator*(Vec3<U> const& other) const
        -> Vec3<decltype(T{} * U{})> {
        return {x * other.x, y * other.y, z * other.z};
    }

    template <typename U>
    constexpr auto operator/(Vec3<U> const& other) const
        -> Vec3<decltype(T{} / U{})> {
        return {x / other.x, y / other.y, z / other.z};
    }

    constexpr Vec3 operator-() const {
        return {-x, -y, -z};
    }

    template <typename U>
    constexpr Vec3<U> cast() const {
        return {
            static_cast<U>(x),
            static_cast<U>(y),
            static_cast<U>(z),
        };
    }

    template <typename U>
    constexpr U into() const {
        return U{x, y, z};
    }

    Vec3 floor() {
        return {Math::floor(x), Math::floor(y), Math::floor(z)};
    }

    Vec3 ceil() {
        return {Math::ceil(x), Math::ceil(y), Math::ceil(z)};
    }

    Vec3 round() {
        return {Math::round(x), Math::round(y), Math::round(z)};
    }

    bool hasNan() const {
        return isNan(x) or isNan(y) or isNan(z);
    }

    auto operator<=>(Vec3 const& other) const {
        return _els <=> other._els;
    }

    bool operator==(Vec3 const& other) const {
        return _els == other._els;
    }

    void repr(Io::Emit& e) const {
        e("(vec {} {} {})", x, y, z);
    }

    constexpr auto map(auto f) const {
        using U = decltype(f(x));
        return Vec3<U>{f(x), f(y), f(z)};
    }
};

template <typename T>
constexpr Vec3<T> operator+(T const& lhs, Vec3<T> const& rhs) {
    return {lhs + rhs.x, lhs + rhs.y, lhs + rhs.z};
}

template <typename T>
constexpr Vec3<T> operator-(T const& lhs, Vec3<T> const& rhs) {
    return {lhs - rhs.x, lhs - rhs.y, lhs - rhs.z};
}

template <typename T>
constexpr Vec3<T> operator*(T const& lhs, Vec3<T> const& rhs) {
    return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z};
}

template <typename T>
constexpr Vec3<T> operator/(T const& lhs, Vec3<T> const& rhs) {
    return {lhs / rhs.x, lhs / rhs.y, lhs / rhs.z};
}

using Vec3i = Vec3<isize>;

using Vec3u = Vec3<usize>;

using Vec3f = Vec3<f64>;

template <typename T>
bool epsilonEq(Vec3<T> const& lhs, Vec3<T> const& rhs, T epsilon) {
    return epsilonEq(lhs.x, rhs.x, epsilon) and
           epsilonEq(lhs.y, rhs.y, epsilon) and
           epsilonEq(lhs.z, rhs.z, epsilon);
}

template <typename T>
union Vec4 {
    using Scalar = T;

    struct {
        T x, y, z, w;
    };

    struct {
        T r, g, b, a;
    };

    Vec3<T> rgb;

    Vec3<T> xyz;

    struct {
        Vec2<T> xy;
        Vec2<T> zw;
    };

    Array<T, 4> _els;

    constexpr Vec4()
        : _els{} {}

    constexpr Vec4(T x, T y, T z, T w)
        : _els{x, y, z, w} {}

    constexpr Vec4(T value)
        : _els{value, value, value, value} {}

    constexpr Vec4(Vec4 const& other)
        : _els{other._els} {}

    constexpr Vec4(Vec4&& other)
        : _els{std::move(other._els)} {}

    constexpr Vec4& operator=(Vec4 const& other) {
        _els = other._els;
        return *this;
    }

    constexpr Vec4& operator=(Vec4&& other) {
        _els = std::move(other._els);
        return *this;
    }

    constexpr ~Vec4() {
        _els.~Array();
    }

    constexpr T min() const {
        return min(x, y, z, w);
    }

    constexpr T max() const {
        return max(x, y, z, w);
    }

    constexpr Vec4 min(Vec4 const& other) {
        return {
            min(x, other.x),
            min(y, other.y),
            min(z, other.z),
            min(w, other.w),
        };
    }

    constexpr Vec4 max(Vec4 const& other) {
        return {
            max(x, other.x),
            max(y, other.y),
            max(z, other.z),
            max(w, other.w),
        };
    }

    constexpr T dot(Vec4 const& other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    constexpr T len() const {
        return sqrt(dot(*this));
    }

    constexpr T dist(Vec4 const& other) const {
        return (*this - other).len();
    }

    constexpr Vec4 unit() const {
        T l = len();
        return {x / l, y / l, z / l, w / l};
    }

    constexpr T operator[](isize i) {
        return _els[i];
    }

    constexpr Vec4 operator+(T const& other) const {
        return {x + other, y + other, z + other, w + other};
    }

    constexpr Vec4 operator-(T const& other) const {
        return {x - other, y - other, z - other, w - other};
    }

    constexpr Vec4 operator*(T const& other) const {
        return {x * other, y * other, z * other, w * other};
    }

    constexpr Vec4 operator/(T const& other) const {
        return {x / other, y / other, z / other, w / other};
    }

    template <typename U>
    constexpr auto operator+(Vec4<U> const& other) const
        -> Vec4<decltype(T{} + U{})> {
        return {x + other.x, y + other.y, z + other.z, w + other.w};
    }

    template <typename U>
    constexpr auto operator-(Vec4<U> const& other) const
        -> Vec4<decltype(T{} - U{})> {
        return {x - other.x, y - other.y, z - other.z, w - other.w};
    }

    template <typename U>
    constexpr auto operator*(Vec4<U> const& other) const
        -> Vec4<decltype(T{} * U{})> {
        return {x * other.x, y * other.y, z * other.z, w * other.w};
    }

    template <typename U>
    constexpr auto operator/(Vec4<U> const& other) const
        -> Vec4<decltype(T{} / U{})> {
        return {x / other.x, y / other.y, z / other.z, w / other.w};
    }

    constexpr Vec4 operator-() const {
        return {-x, -y, -z, -w};
    }

    template <typename U>
    constexpr Vec4<U> cast() const {
        return {
            static_cast<U>(x),
            static_cast<U>(y),
            static_cast<U>(z),
            static_cast<U>(w),
        };
    }

    template <typename U>
    constexpr U into() const {
        return U{x, y, z, w};
    }

    Vec4 floor() {
        return {Math::floor(x), Math::floor(y), Math::floor(z), Math::floor(w)};
    }

    Vec4 ceil() {
        return {Math::ceil(x), Math::ceil(y), Math::ceil(z), Math::ceil(w)};
    }

    Vec4 round() {
        return {Math::round(x), Math::round(y), Math::round(z), Math::round(w)};
    }

    bool hasNan() const {
        return isNan(x) or isNan(y) or isNan(z) or isNan(w);
    }

    auto operator<=>(Vec4 const& other) const {
        return _els <=> other._els;
    }

    bool operator==(Vec4 const& other) const {
        return _els == other._els;
    }

    void repr(Io::Emit& e) const {
        e("(vec {} {} {} {})", x, y, z, w);
    }

    constexpr auto map(auto f) const {
        using U = decltype(f(x));
        return Vec4<U>{f(x), f(y), f(z), f(w)};
    }
};

template <typename T>
constexpr Vec4<T> operator+(T const& lhs, Vec4<T> const& rhs) {
    return {lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w};
}

template <typename T>
constexpr Vec4<T> operator-(T const& lhs, Vec4<T> const& rhs) {
    return {lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w};
}

template <typename T>
constexpr Vec4<T> operator*(T const& lhs, Vec4<T> const& rhs) {
    return {lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w};
}

template <typename T>
constexpr Vec4<T> operator/(T const& lhs, Vec4<T> const& rhs) {
    return {lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w};
}

using Vec4i = Vec4<isize>;

using Vec4u = Vec4<usize>;

using Vec4f = Vec4<f64>;

template <typename T>
bool epsilonEq(Vec4<T> const& lhs, Vec4<T> const& rhs, T epsilon) {
    return epsilonEq(lhs.x, rhs.x, epsilon) and
           epsilonEq(lhs.y, rhs.y, epsilon) and
           epsilonEq(lhs.z, rhs.z, epsilon) and
           epsilonEq(lhs.w, rhs.w, epsilon);
}

} // namespace Karm::Math
