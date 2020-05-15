#pragma once

#include <libmath/math.h>

typedef struct
{
    double x;
    double y;
    double z;
} Vec3f;

static inline Vec3f vec3f_norm(Vec3f vector)
{
    double length = sqrt(vector.x * vector.x +
                         vector.y * vector.y +
                         vector.z * vector.z);

    vector.x /= length;
    vector.y /= length;
    vector.z /= length;

    return vector;
}

static inline Vec3f vec3f_sub(Vec3f left, Vec3f right)
{
    return (Vec3f){left.x - right.x, left.y - right.y, left.z - right.z};
}

static inline Vec3f vec3f_cross(Vec3f left, Vec3f right)
{
    Vec3f result = {};

    result.x = left.y * right.z - left.z * right.y;
    result.y = left.z * right.x - left.x * right.z;
    result.z = left.x * right.y - left.y * right.x;

    result = vec3f_norm(result);

    return result;
}

static inline double vec3f_dot(Vec3f left, Vec3f right)
{
    return left.x * right.x +
           left.y * right.y +
           left.z * right.z;
}
