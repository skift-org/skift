#include <libsystem/math/Math.h>

double tanh(double x)
{
    return __builtin_tanh(x);
}

float tanhf(float x)
{
    return __builtin_tanhf(x);
}