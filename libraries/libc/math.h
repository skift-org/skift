#pragma once

#include <__libc__.h>

#include <libsystem/bits/abs.h>
#include <libsystem/bits/float.h>
#include <libsystem/bits/pi.h>

__BEGIN_HEADER

#define HUGE_VAL (__builtin_huge_val())
#define isinf(x) (__builtin_isinf_sign(x))
#define isfinite(x)  (__builtin_isfinite(x))
#define isnan(x) (__builtin_isnan(x))
#define FP_NAN        0
#define FP_INFINITE   1
#define FP_NORMAL     2
#define FP_SUBNORMAL  3
#define FP_ZERO       4
#define fpclassify(x) (__builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x))
#define hypotf(x, y)  ((float)hypot((float)x,(float)y))

// Double versions
double floor(double x);
double pow(double x, double y);
double exp(double x);
double fmin(double x, double y);
double fmax(double x, double y);
double fmod(double x, double y);
double sqrt(double x);
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
double erf(double x);
double scalbn(double x, int exp);
double scalbln(double x, long int exp);

// Float versions
float floorf(float x);
float fminf(float x, float y);
float fmaxf(float x, float y);
float fmodf(float x, float y);
float sqrtf(float x);
float sinf(float x);
float cosf(float x);
float acosf(float x);
float asinf(float x);
float ceilf(float x);
float logf(float x);
float tanf(float x);
float tanhf(float x);
float erff(float x);
float scalbnf(float x, int exp);
float scalblnf(float x, long int exp);

__END_HEADER
