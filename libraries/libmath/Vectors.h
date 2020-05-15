#pragma once

#include <libmath/vectors/Vec2f.h>
#include <libmath/vectors/Vec2i.h>
#include <libmath/vectors/Vec3f.h>
#include <libmath/vectors/Vec4i.h>

typedef enum
{
    DIM_X = 0,
    DIM_Y = 1,
    DIM_Z = 2,
    DIM_W = 3,
} Dimension;

static inline Dimension dimension_invert_xy(Dimension dimension)
{
    if (dimension == DIM_X)
    {
        return DIM_Y;
    }
    else
    {
        return DIM_X;
    }
}
