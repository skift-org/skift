export module Karm.Core:math.const_;

import :base.base;

namespace Karm::Math {

export constexpr f64 PI = 3.14159265358979323846;

export constexpr f64 TAU = 2 * PI;

// sqrt(2)
export constexpr f64 SQRT2 = 1.41421356237309504880;

// log2(e)
export constexpr f64 L2_E = 1.442695040888963407359924681001892137L;

export constexpr f64 E = 2.71828182845904523536;

export f64 const NAN = 0.0 / 0.0;

export f64 const INF = 1.0 / 0.0;

export f64 const NEG_INF = -1.0 / 0.0;

} // namespace Karm::Math
