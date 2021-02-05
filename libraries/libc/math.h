#pragma once

#include <__libc__.h>

#include <libsystem/bits/abs.h>
#include <libsystem/bits/float.h>
#include <libsystem/bits/pi.h>

__BEGIN_HEADER

#define HUGE_VAL (__builtin_huge_val())
#define isinf(x) (__builtin_isinf_sign(x))
#define isfinite(x) (__builtin_isfinite(x))
#define isnan(x) (__builtin_isnan(x))
#define FP_NAN 0
#define FP_INFINITE 1
#define FP_NORMAL 2
#define FP_SUBNORMAL 3
#define FP_ZERO 4
#define fpclassify(x) (__builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x))

// Min/Max
double fmin(double x, double y);
float fminf(float x, float y);
double fmax(double x, double y);
float fmaxf(float x, float y);

// Arithmetic
double modf(double x, double *iptr);
float modff(float x, float *iptr);
double fmod(double x, double y);
float fmodf(float x, float y);
double sqrt(double x);
float sqrtf(float x);
double pow(double x, double y);

// Trigonometry
double sin(double x);
float sinf(float x);
double asin(double x);
float asinf(float x);
double sinh(double x);
float sinhf(float x);
double cos(double x);
float cosf(float x);
double acos(double x);
float acosf(float x);
double cosh(double x);
float coshf(float x);
double tan(double x);
float tanf(float x);
double atan(double x);
float atanf(float x);
double atan2(double y, double x);
float atan2f(float x, float y);
double tanh(double x);
float tanhf(float x);
double hypot(double x, double y); 
float hypotf(float x, float y);

// Rounding
double ceil(double x);
float ceilf(float x);
double floor(double x);
float floorf(float x);

// Log
double log(double x);
float logf(float x);
double log10(double x);
float log10f(float x);
double log2(double x);
double ldexp(double a, int exp);
float ldexpf(float a, int exp);
double frexp(double x, int *exp);
float frexpf(float x, int *exp);
double exp(double x);
float expf(float x);

// Misc
double erf(double x);
float erff(float x);
double scalbn(double x, int exp);
float scalbnf(float x, int exp);
double scalbln(double x, long int exp);
float scalblnf(float x, long int exp);

__END_HEADER
