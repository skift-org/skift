#pragma once

// Linear interpolation
#define lerp(__start, __end, __transition) ((__start) + ((__end) - (__start)) * (__transition))

// Bilinear interpolation
#define blerp(__p00, __p10, __p01, __p11, __tx, __ty) \
    lerp(lerp((__p00), (__p10), (__tx)), lerp((__p01), (__p11), (__tx)), (__ty))
