#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#define HUGE_VAL (1.0/0.0)

#define PI (3.141592654)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define NAN (0.0 / 0.0)
#define INFINITY (1.0 / 0.0)

#define INF (1.0 / 0.0)
#define POS_INF (1.0 / 0.0)
#define NEG_INF (-1.0 / 0.0)


double floor(double value);
double ceil(double x);

int abs(int value);
double fabs(double value);

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