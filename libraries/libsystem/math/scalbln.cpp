#include <libsystem/math/Math.h>

double scalbln(double x, long int exp)
{
#if __has_builtin(__builtin_scalbln)
    return __builtin_scalbln(x, exp);
#else
#    error "Missing scalbln implementation"
#endif
}

float scalblnf(float x, long int exp)
{
#if __has_builtin(__builtin_scalblnf)
    return __builtin_scalblnf(x, exp);
#else
#    error "Missing scalblnf implementation"
#endif
}