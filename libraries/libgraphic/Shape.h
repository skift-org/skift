#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libmath/MinMax.h>
#include <libsystem/runtime.h>

typedef struct __packed
{
    int X;
    int Y;
} Point;

typedef struct __packed
{
    int top;
    int bottom;
    int left;
    int right;
} Spacing;

typedef union __packed {
    struct
    {
        int X;
        int Y;
        int width;
        int height;
    };

    struct
    {
        Point position;
        Point size;
    };
} Rectangle;

#define RECTANGLE_EMPTY ((Rectangle){})
#define RECTANGLE_SIZE(__width, __height) ((Rectangle){{0, 0, (__width), (__height)}})
#define RECTANGLE(__x, __y, __width, __height) ((Rectangle){{(__x), (__y), (__width), (__height)}})

#define POINT_ZERO ((Point){0, 0})

static inline Point point_add(Point a, Point b)
{
    return (Point){a.X + b.X, a.Y + b.Y};
}

static inline Point point_sub(Point a, Point b)
{
    return (Point){a.X - b.X, a.Y - b.Y};
}

static inline Point point_div(Point a, int value)
{
    return (Point){a.X / value, a.Y / value};
}

static inline Point point_scale(Point a, double scale)
{
    return (Point){a.X * scale, a.Y * scale};
}

static inline Point point_x(Point p)
{
    return (Point){.X = p.X, .Y = 0};
}

static inline Point point_y(Point p)
{
    return (Point){.X = 0, .Y = p.Y};
}

static inline Point point_clamp(Point p, Point pmin, Point pmax)
{
    p.X = MAX(pmin.X, MIN(pmax.X, p.X));
    p.Y = MAX(pmin.Y, MIN(pmax.Y, p.Y));

    return p;
}

static inline Point point_clamp_to_rect(Point p, Rectangle rect)
{
    return point_clamp(p, rect.position, point_add(rect.position, rect.size));
}

static inline Rectangle rectangle_min_size(Rectangle rectangle, Point size)
{
    rectangle.width = MIN(size.X, rectangle.width);
    rectangle.width = MIN(size.Y, rectangle.height);

    return rectangle;
}

static inline Rectangle rectangle_max_size(Rectangle rectangle, Point size)
{
    rectangle.width = MAX(size.X, rectangle.width);
    rectangle.width = MAX(size.Y, rectangle.height);

    return rectangle;
}

static inline bool rectangle_colide(Rectangle a, Rectangle b)
{
    return a.X < b.X + b.width &&
           a.X + a.width > b.X &&
           a.Y < b.Y + b.height &&
           a.height + a.Y > b.Y;
}

static inline Rectangle rectangle_merge(Rectangle a, Rectangle b)
{
    Point topleft;
    topleft.X = MIN(a.X, b.X);
    topleft.Y = MIN(a.Y, b.Y);

    Point bottomright;
    bottomright.X = MAX(a.X + a.width, b.X + b.width);
    bottomright.Y = MAX(a.Y + a.height, b.Y + b.height);

    return (Rectangle){.position = topleft, .size = point_sub(bottomright, topleft)};
}

static inline Rectangle rectangle_clip(Rectangle left, Rectangle right)
{
    int x = MAX(left.X, right.X);
    int y = MAX(left.Y, right.Y);

    int width = MIN(left.X + left.width, right.X + right.width) - x;
    int height = MIN(left.Y + left.height, right.Y + right.height) - y;

    return (Rectangle){{x, y, width, height}};
}

static inline bool rectangle_containe_point(Rectangle rectange, Point position)
{
    return (rectange.X <= position.X && (rectange.X + rectange.width) > position.X) &&
           (rectange.Y <= position.Y && (rectange.Y + rectange.height) > position.Y);
}

static inline Rectangle rectangle_shrink(Rectangle rect, Spacing spacing)
{
    Rectangle result;

    result.X = rect.X + spacing.left;
    result.Y = rect.Y + spacing.top;
    result.width = rect.width - spacing.left - spacing.right;
    result.height = rect.height - spacing.top - spacing.bottom;

    return result;
}

static inline Rectangle rectangle_expand(Rectangle rect, Spacing spacing)
{
    Rectangle result;

    result.X = rect.X - spacing.left;
    result.Y = rect.Y - spacing.top;
    result.width = rect.width + spacing.left + spacing.right;
    result.height = rect.height + spacing.top + spacing.bottom;

    return result;
}

static inline Rectangle rectangle_offset(Rectangle rect, Point offset)
{
    rect.position = point_add(rect.position, offset);

    return rect;
}

static inline bool rectangle_is_empty(Rectangle rect)
{
    return rect.width == 0 && rect.height == 0;
}

static inline Rectangle rectangle_set_height(Rectangle rect, size_t height)
{
    rect.height = height;

    return rect;
}

static inline Rectangle rectangle_set_width(Rectangle rect, size_t width)
{
    rect.width = width;

    return rect;
}