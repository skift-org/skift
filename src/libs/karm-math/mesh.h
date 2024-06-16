#pragma once

#include "tri.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
struct Vert2 {
    using Scalar = T;

    Math::Vec2<T> xy;
    Math::Vec2<T> uv;

    Vert2() = default;

    Vert2(Math::Vec2<T> xy, Math::Vec2<T> uv)
        : xy(xy), uv(uv) {}

    Vert2(T x, T y, T u, T v)
        : xy(x, y), uv(u, v) {}

    Vert2(T x, T y)
        : xy(x, y), uv(0, 0) {}
};

using Vert2i = Vert2<isize>;
using Vert2u = Vert2<usize>;
using Vert2f = Vert2<f64>;

template <typename V, typename T = typename V::Scalar>
struct Mesh2 {
    Vec<usize> index;
    Vec<V> verts;

    static Mesh2<V> plane(f32 width, f32 height, usize widthSegments, usize heightSegments) {
        Mesh2<V> mesh;
        mesh.verts.ensure((widthSegments + 1) * (heightSegments + 1));
        mesh.index.ensure(widthSegments * heightSegments * 6);

        for (usize y = 0; y <= heightSegments; y++) {
            for (usize x = 0; x <= widthSegments; x++) {
                f32 u = static_cast<f32>(x) / widthSegments;
                f32 v = static_cast<f32>(y) / heightSegments;
                mesh.verts.emplaceBack(width * u - width / 2, height * v - height / 2, u, v);
            }
        }

        for (usize y = 0; y < heightSegments; y++) {
            for (usize x = 0; x < widthSegments; x++) {
                mesh.index.pushBack((y + 1) * (widthSegments + 1) + x);
                mesh.index.pushBack(y * (widthSegments + 1) + x);
                mesh.index.pushBack(y * (widthSegments + 1) + x + 1);

                mesh.index.pushBack((y + 1) * (widthSegments + 1) + x);
                mesh.index.pushBack(y * (widthSegments + 1) + x + 1);
                mesh.index.pushBack((y + 1) * (widthSegments + 1) + x + 1);
            }
        }

        return mesh;
    }

    void offset(Vec2<T> off) {
        for (auto &vert : verts)
            vert.xy = vert.xy + off;
    }
};

using Mesh2i = Mesh2<Vert2<isize>>;
using Mesh2u = Mesh2<Vert2<usize>>;
using Mesh2f = Mesh2<Vert2<f64>>;

} // namespace Karm::Math
