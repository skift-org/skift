#include <math.h>

double fmod(double left, double right)
{
#if __has_builtin(__builtin_fmod)
    return __builtin_fmod(left, right);
#else
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
#endif
}

float fmodf(float left, float right)
{
#if __has_builtin(__builtin_fmodf)
    return __builtin_fmodf(left, right);
#else
    if (left >= 0.0f)
    {
        while (left > right)
        {
            left -= right;
        }

        return left;
    }
    else
    {
        return 0.0f;
    }
#endif
}
