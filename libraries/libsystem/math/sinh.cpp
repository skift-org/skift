#include <math.h>

double sinh(double x)
{
#if __has_builtin(__builtin_sinh)
    return __builtin_sinh(x);
#else
#    error "Missing sinh implementation"
#endif
}

float sinhf(float x)
{
#if __has_builtin(__builtin_sinhf)
    return __builtin_sinhf(x);
#else
#    error "Missing sinhf implementation"
#endif
}