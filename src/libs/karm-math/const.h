#pragma once

#include <karm-base/base.h>

namespace Karm::Math {

inline constexpr f64 PI = 3.14159265358979323846;

inline constexpr f64 TAU = 2 * PI;

// sqrt(2)
inline constexpr f64 SQRT2 = 1.41421356237309504880;

// log2(e)
inline constexpr f64 L2_E = 1.442695040888963407359924681001892137L;

inline constexpr f64 E = 2.71828182845904523536;

#undef NAN

inline f64 const NAN = 0.0 / 0.0;

inline f64 const INF = 1.0 / 0.0;

inline f64 const NEG_INF = -1.0 / 0.0;

} // namespace Karm::Math
