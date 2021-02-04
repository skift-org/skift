#include <math.h>

double cosh(double x)
{
#if __has_builtin(__builtin_cosh)
    return __builtin_cosh(x);
#else
#    error "Missing cosh implementation"
#endif
}

float coshf(float x)
{
#if __has_builtin(__builtin_coshf)
    return __builtin_coshf(x);
#else
#    error "Missing coshf implementation"
#endif
}