#include <math.h>

double log(double x)
{
#if __has_builtin(__builtin_log)
    return __builtin_log(x);
#else
#    error "Missing log implementation"
#endif
}

float logf(float x)
{
#if __has_builtin(__builtin_logf)
    return __builtin_logf(x);
#else
#    error "Missing logf implementation"
#endif
}
