#include <math.h>

double fmin(double left, double right)
{
    return left < right ? left : right;
}

float fminf(float left, float right)
{
    return left < right ? left : right;
}