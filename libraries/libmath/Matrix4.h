#pragma once

#include <libmath/Vectors.h>

typedef struct
{
    double m[4][4];
} Matrix4;

Matrix4 matrix_create_projection(double near, double far, double fov, double aspect_ratio);

Matrix4 matrix_create_rotationX(double theta);

// Matrix4 matrix_create_rotationY(double theta);

Matrix4 matrix_create_rotationZ(double theta);

Vec3f matrix_apply_tranform(Matrix4 matrix, Vec3f vector);
