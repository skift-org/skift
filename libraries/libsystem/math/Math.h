#pragma once

#include <__libc__.h>

#include <libsystem/Common.h>

#include <libsystem/bits/abs.h>
#include <libsystem/bits/float.h>
#include <libsystem/bits/pi.h>

__BEGIN_HEADER

double asin(double x);
double atan2(double y, double x);
double ceil(double x);

double cos(double x);
static inline float cosf(float x) { return (float)cos((double)x); }

double acos(double x);
static inline float acosf(float x) { return (float)acos((double)x); }

double sin(double x);
static inline float sinf(float x) { return (float)sin((double)x); }

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

double sinh(double x);

double sqrt(double x);
static inline float sqrtf(float x) { return (float)sqrt((double)x); }

double tan(double x);
static inline float tanf(float x) { return (float)tan((double)x); }

double tanh(double x);

double hypot(double x, double y);

float atan2f(float y, float x);
float ceilf(float x);
float cosf(float x);
float floorf(float x);
float fmodf(float x, float y);
float sinf(float x);

#define HUGE_VAL (__builtin_huge_val())
#define isinf(x) __builtin_isinf_sign(x)
#define isfinite(x)  (__builtin_isfinite(x))

__END_HEADER
