#include <math.h>

double log10(double x)
{
#if __has_builtin(__builtin_log10)
    return __builtin_log10(x);
#else
#    error "Missing log10 implementation"
#endif
}

float log10f(float x)
{
#if __has_builtin(__builtin_log10f)
    return __builtin_log10f(x);
#else
#    error "Missing log10f implementation"
#endif
}
