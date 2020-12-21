#define __abs(x) ((x) < 0 ? -(x) : (x))

int abs(int x)
{
    return __abs(x);
}

long labs(long x)
{
    return __abs(x);
}

long long llabs(long long x)
{
    return __abs(x);
}

double fabs(double x)
{
    return __abs(x);
}

float fabsf(float x)
{
    return __abs(x);
}

long double fabsl(long double x)
{
    return __abs(x);
}
