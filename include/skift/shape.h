#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/math.h>

typedef struct attr_packed
{
    int X;
    int Y;
} point_t;

typedef struct attr_packed
{
    int top;
    int bottom;
    int left;
    int right;
} spacing_t;

typedef union attr_packed {
    struct
    {
        int X;
        int Y;
        int width;
        int height;
    };

    struct
    {
        point_t position;
        point_t size;
    };
} rectangle_t;

#define point_zero ((point_t){0, 0})

static inline point_t point_add(point_t a, point_t b)
{
    return (point_t){a.X + b.X, a.Y + b.Y};
}

static inline point_t point_sub(point_t a, point_t b)
{
    return (point_t){a.X - b.X, a.Y - b.Y};
}

static inline point_t point_div(point_t a, int value)
{
    return (point_t){a.X / value, a.Y / value};
}

static inline point_t point_scale(point_t a, double scale)
{
    return (point_t){a.X * scale, a.Y * scale};
}

static inline point_t point_x(point_t p)
{
    return (point_t){.X = p.X, .Y = 0};
}

static inline point_t point_y(point_t p)
{
    return (point_t){.X = 0, .Y = p.Y};
}

static inline point_t point_clamp(point_t p, point_t pmin, point_t pmax)
{
    p.X = max(pmin.X, min(pmax.X, p.X));
    p.Y = max(pmin.Y, min(pmax.Y, p.Y));

    return p;
}

static inline point_t point_clamp_to_rect(point_t p, rectangle_t rect)
{
    return point_clamp(p, rect.position, point_add(rect.position, rect.size));
}

static inline rectangle_t rectangle_min_size(rectangle_t rectangle, point_t size)
{
    rectangle.width = min(size.X, rectangle.width);
    rectangle.width = min(size.Y, rectangle.height);

    return rectangle;
}

static inline rectangle_t rectangle_max_size(rectangle_t rectangle, point_t size)
{
    rectangle.width = max(size.X, rectangle.width);
    rectangle.width = max(size.Y, rectangle.height);

    return rectangle;
}

static inline bool rectange_colide(rectangle_t a, rectangle_t b)
{
    return a.X < b.X + b.width &&
           a.X + a.width > b.Y &&
           a.Y < b.Y + b.height &&
           a.height + a.Y > b.Y;
}

static inline rectangle_t rectangle_merge(rectangle_t a, rectangle_t b)
{
    point_t topleft;
    topleft.X = min(a.X, b.X);
    topleft.Y = min(a.Y, b.Y);

    point_t bottomright;
    bottomright.X = max(a.X + a.width, b.X + b.width);
    bottomright.Y = max(a.Y + a.height, b.Y + b.height);

    return (rectangle_t){.position = topleft, .size = point_sub(bottomright, topleft)};
}

static inline rectangle_t rectangle_child(rectangle_t rectangle, rectangle_t child_rectangle)
{
    int x = max(rectangle.X + child_rectangle.X, rectangle.X);
    int y = max(rectangle.Y + child_rectangle.Y, rectangle.Y);

    int width = min(rectangle.X + child_rectangle.X + child_rectangle.width, rectangle.X + rectangle.width) - x;
    int height = min(rectangle.Y + child_rectangle.Y + child_rectangle.height, rectangle.Y + rectangle.height) - y;

    return (rectangle_t){x, y, width, height};
}

static inline bool rectangle_containe_position(rectangle_t rectange, point_t position)
{
    return (rectange.X <= position.X && (rectange.X + rectange.width) > position.X) &&
           (rectange.Y <= position.Y && (rectange.Y + rectange.height) > position.Y);
}

static inline rectangle_t rectangle_shrink(rectangle_t rect, spacing_t spacing)
{
    rectangle_t result;

    result.X = rect.X + spacing.left;
    result.Y = rect.Y + spacing.top;
    result.width = rect.width - spacing.left - spacing.right;
    result.height = rect.height - spacing.top - spacing.bottom;

    return result;
}

static inline rectangle_t rectangle_expand(rectangle_t rect, spacing_t spacing)
{
    rectangle_t result;

    result.X = rect.X - spacing.left;
    result.Y = rect.Y - spacing.top;
    result.width = rect.width + spacing.left + spacing.right;
    result.height = rect.height + spacing.top + spacing.bottom;

    return result;
}
