#include <libsystem/math/Math.h>

#include "sin_data.h"

double sin(double x)
{
    if (x < 0.0)
    {
        x += 3.141592654 * 2.0 * 100.0;
    }

    int i = x * 360.0 / (3.141592654 * 2.0);
    double z = x * 360.0 / (3.141592654 * 2.0);
    z -= i;

    i = i % 360;

    double a = sin_table[i];
    double b = sin_table[(i + 1) % 360];

    return a * (1.0 - z) + b * (z);
}
