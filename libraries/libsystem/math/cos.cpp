#include <libsystem/math/Math.h>

double cos(double x)
{
#if __has_builtin(__builtin_sqrt)
    return __builtin_cos(x);
#else
    return sin(x + M_PI_2);
#endif
}

float cosf(float x)
{
#if __has_builtin(__builtin_cosf)
    return __builtin_cosf(x);
#else
    return sinf(x + M_PI_2);
#endif
}