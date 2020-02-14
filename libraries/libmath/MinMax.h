#pragma once


#define clamp(__v, __lower, __upper) (max(min((__v), (__upper)), (__lower)))

#define min(__x, __y) ((__x) < (__y) ? (__x) : (__y))

#define max(__x, __y) ((__x) > (__y) ? (__x) : (__y))