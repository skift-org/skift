#include <libsystem/math/Math.h>

double hypot(double x, double y)
{
    if (isfinite(x) && isfinite(y))
    {
        /* Determine absolute values.  */
        x = fabs(x);
        y = fabs(y);

        {
            /* Find the bigger and the smaller one.  */
            double a;
            double b;

            if (x >= y)
            {
                a = x;
                b = y;
            }
            else
            {
                a = y;
                b = x;
            }
            /* Now 0 <= b <= a.  */

            {
                int e;
                double an;
                double bn;

                /* Write a = an * 2^e, b = bn * 2^e with 0 <= bn <= an < 1.  */
                an = frexp(a, &e);
                bn = ldexp(b, -e);

                {
                    double cn;

                    /* Through the normalization, no unneeded overflow or underflow
               will occur here.  */
                    cn = sqrt(an * an + bn * bn);
                    return ldexp(cn, e);
                }
            }
        }
    }
    else
    {
        if (isinf(x) || isinf(y))
            /* x or y is infinite.  Return +Infinity.  */
            return HUGE_VAL;
        else
            /* x or y is NaN.  Return NaN.  */
            return x + y;
    }
}