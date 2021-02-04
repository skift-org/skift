#include <math.h>

double atan2(double x, double y)
{
#if __has_builtin(__builtin_atan2)
    return __builtin_atan2(x, y);
#else
#    error "Missing atan2 implementation"
#endif
}

float atan2f(float x, float y)
{
#if __has_builtin(__builtin_atan2f)
    return __builtin_atan2f(x, y);
#else
#    error "Missing atan2f implementation"
#endif
}