#pragma once

#include "vec.h"

namespace Karm::Math {

// Matrices types, column major, post multiply

template <typename T>
union Mat2 {
    using Scalar = T;

    struct {
        T m00, m01;
        T m10, m11;
    };

    struct {
        Vec2<T> col0;
        Vec2<T> col1;
    };

    Array<Vec2<T>, 2> cols;
    Array<T, 2 * 2> _els;

    static constexpr Mat2 identity() {
        return {
            {1, 0},
            {0, 1}
        };
    }

    Mat2() = default;

    Mat2(T m00, T m01, T m10, T m11)
        : _els{m00, m01, m10, m11} {}

    Mat2(Vec2<T> col0, Vec2<T> col1)
        : cols{col0, col1} {}

    void repr(Io::Emit& e) const {
        e("(mat2 ");
        for (auto& col : cols) {
            e("{}", col);
        }
        e(")");
    }
};

using Mat2i = Mat2<i64>;
using Mat2f = Mat2<f64>;

template <typename T>
union Mat3 {
    using Scalar = T;

    struct {
        T m00, m01, m02;
        T m10, m11, m12;
        T m20, m21, m22;
    };

    struct {
        Vec3<T> col0;
        Vec3<T> col1;
        Vec3<T> col2;
    };

    Array<Vec3<T>, 3> cols;
    Array<T, 3 * 3> _els;

    Mat3() = default;

    Mat3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22)
        : _els{m00, m01, m02, m10, m11, m12, m20, m21, m22} {}

    Mat3(Vec3<T> col0, Vec3<T> col1, Vec3<T> col2)
        : cols{col0, col1, col2} {}

    void repr(Io::Emit& e) const {
        e("(mat3 ");
        for (auto& col : cols) {
            e("{}", col);
        }
        e(")");
    }
};

using Mat3i = Mat3<i64>;
using Mat3f = Mat3<f64>;

template <typename T>
union Mat4 {
    using Scalar = T;

    struct {
        T m00, m01, m02, m03;
        T m10, m11, m12, m13;
        T m20, m21, m22, m23;
        T m30, m31, m32, m33;
    };

    struct {
        Vec4<T> col0;
        Vec4<T> col1;
        Vec4<T> col2;
        Vec4<T> col3;
    };

    Array<Vec4<T>, 4> cols;
    Array<T, 4 * 4> _els;

    Mat4() = default;

    Mat4(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13, T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33)
        : _els{m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33} {}

    Mat4(Vec4<T> col0, Vec4<T> col1, Vec4<T> col2, Vec4<T> col3)
        : cols{col0, col1, col2, col3} {}

    void repr(Io::Emit& e) const {
        e("(mat4 ");
        for (auto& col : cols) {
            e("{}", col);
        }
        e(")");
    }
};

using Mat4i = Mat4<i64>;
using Mat4f = Mat4<f64>;

} // namespace Karm::Math
