#include <libsystem/bits/pi.h>
#include <math.h>

#include "sin_data.h"

double sin(double x)
{
#if __has_builtin(__builtin_sinf)
    return __builtin_sin(x);
#else
    if (x < 0.0)
    {
        x += M_PI * 2.0 * 100.0;
    }

    int i = x * 360.0 / (M_PI * 2.0);
    double z = x * 360.0 / (M_PI * 2.0);
    z -= i;

    i = i % 360;

    double a = sin_table[i];
    double b = sin_table[(i + 1) % 360];

    return a * (1.0 - z) + b * (z);
#endif
}

float sinf(float x)
{
#if __has_builtin(__builtin_sinf)
    return __builtin_sinf(x);
#else
    if (x < 0.0)
    {
        x += M_PI * 2.0 * 100.0;
    }

    int i = x * 360.0 / (M_2_PI);
    float z = x * 360.0 / (M_2_PI);
    z -= i;

    i = i % 360;

    float a = sin_table[i];
    float b = sin_table[(i + 1) % 360];

    return a * (1.0 - z) + b * (z);
#endif
}
