#include <libsystem/math/Math.h>

double floor(double x)
{
#if __has_builtin(__builtin_floor)
    return __builtin_floor(x);
#else
    if (x > -1.0 && x < 1.0)
    {
        return x >= 0 ? 0.0 : -1.0;
    }

    if (x < 0)
    {
        int x_i = x;
        return (double)(x_i - 1);
    }

    int x_i = x;
    return (double)x_i;
#endif
}

float floor(float x)
{
#if __has_builtin(__builtin_floorf)
    return __builtin_floorf(x);
#else
    if (x > -1.0f && x < 1.0f)
    {
        return x >= 0 ? 0.0f : -1.0f;
    }

    if (x < 0)
    {
        int x_i = x;
        return (float)(x_i - 1);
    }

    int x_i = x;
    return (float)x_i;
#endif
}
