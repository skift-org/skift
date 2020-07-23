#pragma once

#include <__libc__.h>

#include <libsystem/Common.h>

__BEGIN_HEADER

#define M_PI 3.1415926
#define PI (3.141592654)

#define NAN (0.0 / 0.0)
#define INFINITY (1.0 / 0.0)

#define INF (1.0 / 0.0)
#define POS_INF (1.0 / 0.0)
#define NEG_INF (-1.0 / 0.0)

#define abs(x) ((x) < 0 ? -(x) : (x))
#define fabs(x) abs(x)

double acos(double x);
double asin(double x);
double atan2(double y, double x);
double ceil(double x);
double cos(double x);
double cosh(double x);
double exp(double x);
double floor(double x);
double fmod(double x, double y);
double frexp(double x, int *exp);
double ldexp(double x, int exp);
double log(double x);
double log10(double x);
double log2(double x);
double pow(double x, double y);
double sin(double x);
double sinh(double x);
double sqrt(double x);
double tan(double x);
double tanh(double x);
float acosf(float x);
float atan2f(float y, float x);
float ceilf(float x);
float cosf(float x);
float floorf(float x);
float fmodf(float x, float y);
float sinf(float x);
float sqrtf(float arg);

__END_HEADER
