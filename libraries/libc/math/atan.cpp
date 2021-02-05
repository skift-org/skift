#include <math.h>

double atan(double x)
{
#if __has_builtin(__builtin_atan)
    return __builtin_atan(x);
#else
#    error "Missing atan implementation"
#endif
}

float atanf(float x)
{
#if __has_builtin(__builtin_atanf)
    return __builtin_atanf(x);
#else
#    error "Missing atanf implementation"
#endif
}