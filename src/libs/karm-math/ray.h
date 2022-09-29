#pragma once

#include "vec.h"

namespace Karm::Math {

template <typename V, typename T = typename V::Scalar>
struct Ray {
    V origin;
    V direction;

    constexpr Ray() = default;

    constexpr Ray(V origin, V direction) : origin(origin), direction(direction) {}

    constexpr V at(T t) const {
        return origin + direction * t;
    }
};

using Rayi = Ray<int, int>;

using Rayf = Ray<float, float>;

using Ray2i = Ray<Vec2i>;

using Ray2f = Ray<Vec2f>;

using Ray3i = Ray<Vec3i>;

using Ray3f = Ray<Vec3f>;

} // namespace Karm::Math
