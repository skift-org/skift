#pragma once

namespace Math
{

#define clamp(__v, __lower, __upper) (MAX(MIN((__v), (__upper)), (__lower)))

#define MIN(__x, __y) ((__x) < (__y) ? (__x) : (__y))

#define MAX(__x, __y) ((__x) > (__y) ? (__x) : (__y))

} // namespace Math
