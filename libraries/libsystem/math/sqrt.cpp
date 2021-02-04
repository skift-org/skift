#include <libsystem/math/Math.h>

double sqrt(double x)
{
#if __has_builtin(__builtin_sqrt)
    return __builtin_sqrt(x);
#else
#    error "Missing sqrt implementation"
#endif
}

float sqrtf(float x)
{
#if __has_builtin(__builtin_sqrtf)
    return __builtin_sqrtf(x);
#else
#    error "Missing sqrtf implementation"
#endif
}