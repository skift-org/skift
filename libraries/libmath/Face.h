#pragma once

#include <libmath/Vector3.h>

typedef struct
{
    Vector3 a;
    Vector3 b;
    Vector3 c;
} Face;

Vector3 face_normal(Face face);