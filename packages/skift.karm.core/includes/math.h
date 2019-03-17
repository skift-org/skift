#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#define PI (3.141592654)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define NAN (0.0 / 0.0)
#define INFINITY (1.0 / 0.0)

#define INF (1.0 / 0.0)
#define POS_INF (1.0 / 0.0)
#define NEG_INF (-1.0 / 0.0)

int abs(int value);

double floor(double value);
double fabs(double value);
double fmod(double x, double y);
double sin(double value);
double cos(double value);
