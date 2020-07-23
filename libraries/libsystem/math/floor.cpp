#include <libsystem/math/Math.h>

double floor(double x)
{
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
}
