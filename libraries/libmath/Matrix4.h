#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libmath/Vector3.h>

typedef struct
{
    double m[4][4];
} Matrix4;

Matrix4 matrix_create_projection(double near, double far, double fov, double aspect_ratio);

Matrix4 matrix_create_rotationX(double theta);

// Matrix4 matrix_create_rotationY(double theta);

Matrix4 matrix_create_rotationZ(double theta);

Vector3 matrix_apply_tranform(Matrix4 matrix, Vector3 vector);