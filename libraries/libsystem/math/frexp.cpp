#include <math.h>
#include <stdint.h>

double frexp(double x, int *exp)
{
#if __has_builtin(_builtin_frexp)
    return _builtin_frexp(x, exp);
#else
    union
    {
        double d;
        uint64_t i;
    } y = {x};
    int ee = y.i >> 52 & 0x7ff;

    if (!ee)
    {
        if (x)
        {
            x = frexp(x * 0x1p64, exp);
            *exp -= 64;
        }
        else
            *exp = 0;
        return x;
    }
    else if (ee == 0x7ff)
    {
        return x;
    }

    *exp = ee - 0x3fe;
    y.i &= 0x800fffffffffffffull;
    y.i |= 0x3fe0000000000000ull;
    return y.d;
#endif
}

float frexpf(float x, int *exp)
{
#if __has_builtin(_builtin_frexpf)
    return _builtin_frexpf(x, exp);
#else
    union
    {
        float f;
        uint32_t i;
    } y = {x};
    int ee = y.i >> 23 & 0xff;

    if (!ee)
    {
        if (x)
        {
            x = frexpf(x * 0x1p64, exp);
            *exp -= 64;
        }
        else
            *exp = 0;
        return x;
    }
    else if (ee == 0xff)
    {
        return x;
    }

    *exp = ee - 0x7e;
    y.i &= 0x807ffffful;
    y.i |= 0x3f000000ul;
    return y.f;
#endif
}