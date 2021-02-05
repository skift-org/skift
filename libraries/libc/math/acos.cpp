#include <math.h>

double acos(double x)
{
#if __has_builtin(__builtin_acos)
    return __builtin_acos(x);
#else
    return (-0.69813170079773212 * x * x - 0.87266462599716477) * x + M_PI_2;
#endif
}

float acosf(float x)
{
#if __has_builtin(__builtin_acosf)
    return __builtin_acosf(x);
#else
    return (-0.69813170079773212 * x * x - 0.87266462599716477) * x + M_PI_2;
#endif
}