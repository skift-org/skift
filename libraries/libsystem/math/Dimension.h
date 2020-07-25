#pragma once

enum class Dimension
{
    X,
    Y,
};

static inline Dimension dimension_invert_xy(Dimension dimension)
{
    if (dimension == Dimension::X)
    {
        return Dimension::Y;
    }
    else
    {
        return Dimension::X;
    }
}
