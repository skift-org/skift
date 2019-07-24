#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>

#define HUGE_VAL (1.0 / 0.0)

#define PI (3.141592654)

#define clamp(__v, __lower, __upper) max(min((__v), (__upper)), (__lower))

#define min(__x, __y) ((__x) < (__y) ? (__x) : (__y))

#define max(__x, __y) ((__x) > (__y) ? (__x) : (__y))

#define clamp(__x, __min, __max) (min((__max), max((__min), (__x))))

// Linear interpolation
#define lerp(__start, __end, __transition) ((__start) + ((__end) - (__start)) * (__transition))

// Bilinear interpolation
#define blerp(__p00, __p10, __p01, __p11, __tx, __ty) \
    lerp(lerp((__p00), (__p10), (__tx)), lerp((__p01), (__p11), (__tx)), (__ty))

#define NAN (0.0 / 0.0)
#define INFINITY (1.0 / 0.0)

#define INF (1.0 / 0.0)
#define POS_INF (1.0 / 0.0)
#define NEG_INF (-1.0 / 0.0)

double floor(double value);
double ceil(double x);

#define abs(__value) ((__value) < 0 ? -(__value) : (__value))

double fmod(double x, double y);

double sin(double value);
double asin(double x);
double sinh(double x);

double cos(double value);
double acos(double x);
double cosh(double x);

double tan(double x);
double tanh(double x);
double atan2(double y, double x);

double pow(double x, double y);
double log(double x);
double log10(double x);
double log2(double x);

double exp(double x);
double ldexp(double x, int exp);
double frexp(double x, int *exponent);

double sqrt(double x);