#pragma once

#include <libmath/math.h>

typedef struct
{
    double m[4][4];
} matrix_t;

matrix_t matrix_create_projection(double near, double far, double fov, double aspect_ratio)
{
    double fov_rad = 1.0 / tan(fov * 0.5 / 180.0 * PI);
    matrix_t projection = {0};

    projection.m[0][0] = aspect_ratio * fov_rad;
    projection.m[1][1] = fov_rad;
    projection.m[2][2] = far / (far - near);
    projection.m[3][2] = (-far * near) / (far - near);
    projection.m[2][3] = 1.0;
    projection.m[3][3] = 0.0;

    return projection;
}

matrix_t matrix_create_rotationX(double theta)
{
    matrix_t mat = {0};

    mat.m[0][0] = 1;
    mat.m[1][1] = cos(theta * 0.5);
    mat.m[1][2] = sin(theta * 0.5);
    mat.m[2][1] = -sin(theta * 0.5);
    mat.m[2][2] = cos(theta * 0.5);
    mat.m[3][3] = 1;

    return mat;
}

// matrix_t matrix_create_rotationY(double theta)
// {
//
// }

matrix_t matrix_create_rotationZ(double theta)
{
    matrix_t mat = {0};

    mat.m[0][0] = cos(theta);
    mat.m[0][1] = sin(theta);
    mat.m[1][0] = -sin(theta);
    mat.m[1][1] = cos(theta);
    mat.m[2][2] = 1;
    mat.m[3][3] = 1;

    return mat;
}