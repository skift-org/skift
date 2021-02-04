#pragma once

#include <__libc__.h>

#include <libsystem/Common.h>

#include <libsystem/bits/abs.h>
#include <libsystem/bits/float.h>
#include <libsystem/bits/pi.h>

__BEGIN_HEADER

double asin(double x);
float asinf(float x);

double atan2(double y, double x);
float atan2f(float y, float x);

double ceil(double x);
float ceilf(float x);

double cos(double x);
float cosf(float x);

double acos(double x);
float acosf(float x);

double asin(double x);
float asinf(float x);

double sin(double x);
float sinf(float x);

double cosh(double x);
double exp(double x);

double floor(double x);
float floorf(float x);

double fmod(double x, double y);
float fmodf(float x, float y);

double frexp(double x, int *exp);
double ldexp(double x, int exp);
double log(double x);
double log10(double x);
double log2(double x);
double pow(double x, double y);

double fmin(double x, double y);
float fminf(float x, float y);

double fmax(double x, double y);
float fmaxf(float x, float y);

double sinh(double x);

double sqrt(double x);
float sqrtf(float x);

double tan(double x);
float tanf(float x);

double tanh(double x);
float tanhf(float x);

double erf(double x);
float erff(float x);

double scalbn(double x, int exp);
float scalbnf(float x, int exp);

double scalbln(double x, long int exp);
float scalblnf(float x, long int exp);

#define HUGE_VAL (__builtin_huge_val())
#define isinf(x) __builtin_isinf_sign(x)
#define isfinite(x)  (__builtin_isfinite(x))

__END_HEADER
