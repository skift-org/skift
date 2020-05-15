#pragma once

#include <libmath/math.h>

typedef struct
{
    double X;
    double Y;
    double Z;
} Vec3f;

static inline Vec3f vec3f_norm(Vec3f vector)
{
    double length = sqrt(vector.X * vector.X +
                         vector.Y * vector.Y +
                         vector.Z * vector.Z);

    vector.X /= length;
    vector.Y /= length;
    vector.Z /= length;

    return vector;
}

static inline Vec3f vec3f_sub(Vec3f left, Vec3f right)
{
    return (Vec3f){left.X - right.X, left.Y - right.Y, left.Z - right.Z};
}

static inline Vec3f vec3f_cross(Vec3f left, Vec3f right)
{
    Vec3f result = {};

    result.X = left.Y * right.Z - left.Z * right.Y;
    result.Y = left.Z * right.X - left.X * right.Z;
    result.Z = left.X * right.Y - left.Y * right.X;

    result = vec3f_norm(result);

    return result;
}

static inline double vec3f_dot(Vec3f left, Vec3f right)
{
    return left.X * right.X +
           left.Y * right.Y +
           left.Z * right.Z;
}
