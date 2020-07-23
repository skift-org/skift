#pragma once

#include <__libc__.h>

__BEGIN_HEADER

#define M_PI 3.1415926
#define PI (3.141592654)

#define HUGE_VAL (__builtin_huge_val())

#define NAN (0.0 / 0.0)
#define INFINITY (1.0 / 0.0)
#define INF (1.0 / 0.0)
#define POS_INF (1.0 / 0.0)
#define NEG_INF (-1.0 / 0.0)

double floor(double x);
int abs(int j);
double pow(double x, double y);
double exp(double x);
double fmod(double x, double y);
double sqrt(double x);
float sqrtf(float x);
double fabs(double x);
float fabsf(float x);
double sin(double x);
double cos(double x);
double frexp(double x, int *exp);
double acos(double x);
double asin(double x);
double atan2(double y, double x);
double ceil(double x);
double cosh(double x);
double ldexp(double a, int exp);
double log(double x);
double log10(double x);
double log2(double x);
double sinh(double x);
double tan(double x);
double tanh(double x);
double atan(double x);
double modf(double x, double *iptr);
double hypot(double x, double y);

__END_HEADER
