#include <libsystem/math/Math.h>

double scalbn(double x, long int exp)
{
#if __has_builtin(__builtin_scalbn)
    return __builtin_scalbn(x, exp);
#else
#    error "Missing scalbn implementation"
#endif
}

float scalbnf(float x, long int exp)
{
#if __has_builtin(__builtin_scalbnf)
    return __builtin_scalbnf(x, exp);
#else
#    error "Missing scalbnf implementation"
#endif
}