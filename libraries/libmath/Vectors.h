#pragma once

#include <libmath/vectors/Vec2f.h>
#include <libmath/vectors/Vec2i.h>

typedef enum
{
    DIM_X = 0,
    DIM_Y = 1,
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
