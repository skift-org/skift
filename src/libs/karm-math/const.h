#pragma once

#include <karm-base/base.h>

namespace Karm::Math {

inline constexpr f64 PI = 3.14159265358979323846;

inline constexpr f64 TAU = 2 * PI;

inline constexpr f64 SQRT = 1.41421356237309504880;

inline constexpr f64 E = 2.71828182845904523536;

#undef NAN

inline f64 const NAN = 0.0 / 0.0;

inline f64 const INF = 1.0 / 0.0;

inline f64 const NEG_INF = -1.0 / 0.0;

} // namespace Karm::Math
