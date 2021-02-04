#include <math.h>

double exp(double x)
{
#if __has_builtin(__builtin_exp)
    return __builtin_exp(x);
#else
#    error "Missing exp implementation"
#endif
}

float expf(float x)
{
#if __has_builtin(__builtin_expf)
    return __builtin_expf(x);
#else
#    error "Missing expf implementation"
#endif
}