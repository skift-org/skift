#include <libsystem/math/Math.h>

double fmod(double left, double right)
{
    if (left >= 0.0)
    {
        while (left > right)
        {
            left -= right;
        }

        return left;
    }
    else
    {
        return 0.0;
    }
}
