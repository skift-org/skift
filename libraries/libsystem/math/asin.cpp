#include <libsystem/math/Math.h>

double asin(double x)
{
#if __has_builtin(__builtin_asin)
    return __builtin_asin(x);
#else
#    error "Missing asin implementation"
#endif
}

float asinf(float x)
{
#if __has_builtin(__builtin_asinf)
    return __builtin_asinf(x);
#else
#    error "Missing asinf implementation"
#endif
}