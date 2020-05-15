#pragma once

#include <libmath/Vectors.h>

typedef struct
{
    Vec3f a;
    Vec3f b;
    Vec3f c;
} Face;

Vec3f face_normal(Face face);