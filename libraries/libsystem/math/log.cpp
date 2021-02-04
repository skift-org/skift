#include <math.h>

double log(double x)
{
#if __has_builtin(__builtin_log)
    return __builtin_log(x);
#else
#    error "Missing log implementation"
#endif
}
