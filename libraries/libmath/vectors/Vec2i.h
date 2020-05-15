#pragma once

#include <libmath/MinMax.h>
#include <libsystem/Common.h>

typedef union
{
    struct
    {
        int x, y;
    };

    struct
    {
        int width, height;
    };

    int components[2];
} Vec2i;

#define vec2i(__x, __y) ((Vec2i){{(__x), (__y)}})

#define vec2i_zero vec2i(0, 0)

static inline Vec2i vec2i_add(Vec2i a, Vec2i b)
{
    return vec2i(a.x + b.x, a.y + b.y);
}

static inline Vec2i vec2i_sub(Vec2i a, Vec2i b)
{
    return vec2i(a.x - b.x, a.y - b.y);
}

static inline Vec2i vec2i_div(Vec2i a, int value)
{
    return vec2i(a.x / value, a.y / value);
}

static inline bool vec2i_equ(Vec2i lhs, Vec2i rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

static inline Vec2i vec2i_scale(Vec2i a, double scale)
{
    return vec2i((int)(a.x * scale), (int)(a.y * scale));
}

static inline Vec2i vec2i_x(Vec2i p)
{
    return vec2i(p.x, 0);
}

static inline Vec2i vec2i_y(Vec2i p)
{
    return vec2i(0, p.y);
}

static inline Vec2i vec2i_clamp(Vec2i p, Vec2i pmin, Vec2i pmax)
{
    p.x = MAX(pmin.x, MIN(pmax.x, p.x));
    p.y = MAX(pmin.y, MIN(pmax.y, p.y));

    return p;
}
