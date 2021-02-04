#include <libsystem/math/Math.h>

double erf(double x)
{
#if __has_builtin(__builtin_erf)
    return __builtin_erf(x);
#else
#    error "No erf implementation"
#endif
}

float erff(float x)
{
#if __has_builtin(__builtin_erff)
    return __builtin_erff(x);
#else
#    error "No erff implementation"
#endif
}