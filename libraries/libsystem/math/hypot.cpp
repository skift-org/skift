#include <math.h>

double hypot(double x, double y)
{
#if __has_builtin(__builtin_hypot)
    return __builtin_hypot(x, y);
#else
#    error "Missing hypot implementation"
#endif
}

float hypotf(float x, float y)
{
#if __has_builtin(__builtin_hypotf)
    return __builtin_hypotf(x, y);
#else
#    error "Missing hypotf implementation"
#endif
}