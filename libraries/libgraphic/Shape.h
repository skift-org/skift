#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libmath/MinMax.h>
#include <libmath/math.h>

typedef struct __packed
{
    int X;
    int Y;
} Point;

typedef struct __packed
{
    double X;
    double Y;
} PointF;

typedef struct __packed
{
    int top;
    int bottom;
    int left;
    int right;
} Insets;

#define __INSETS1(__tblr) ((Insets){.top = __tblr, .bottom = __tblr, .left = __tblr, .right = __tblr})
#define __INSETS2(__tb, __lr) ((Insets){.top = __tb, .bottom = __tb, .left = __lr, .right = __lr})
#define __INSETS3(__t, __b, __lr) ((Insets){.top = __t, .bottom = __b, .left = __lr, .right = __lr})
#define __INSETS4(__t, __b, __l, __r) ((Insets){__t, __b, __l, __r})

#define __INSETS(_1, _2, _3, _4, NAME, ...) NAME

#define INSETS(...)                                                   \
    __INSETS(__VA_ARGS__, __INSETS4, __INSETS3, __INSETS2, __INSETS1) \
    (__VA_ARGS__)

typedef union __packed
{
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

static inline bool point_equ(Point lhs, Point rhs)
{
    return lhs.X == rhs.X && lhs.Y == rhs.Y;
}

static inline Point point_scale(Point a, double scale)
{
    return (Point){(int)(a.X * scale), (int)(a.Y * scale)};
}

static inline Point point_x(Point p)
{
    return (Point){p.X, 0};
}

static inline Point point_y(Point p)
{
    return (Point){0, p.Y};
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

static inline Rectangle rectangle_from_two_point(Point a, Point b)
{
    return (Rectangle){{
        MIN(a.X, b.X),
        MIN(a.Y, b.Y),
        abs(a.X - b.X),
        abs(a.Y - b.Y),
    }};
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

    Rectangle rectangle = {};

    rectangle.position = topleft;
    rectangle.size = point_sub(bottomright, topleft);

    return rectangle;
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

static inline bool rectangle_container_rectangle(Rectangle rectange, Rectangle contained)
{
    return (rectange.X <= contained.X && (contained.X + contained.width) >= contained.X) &&
           (rectange.Y <= contained.Y && (contained.Y + contained.height) >= contained.Y);
}

static inline Rectangle rectangle_shrink(Rectangle rect, Insets spacing)
{
    Rectangle result;

    result.X = rect.X + spacing.left;
    result.Y = rect.Y + spacing.top;
    result.width = rect.width - spacing.left - spacing.right;
    result.height = rect.height - spacing.top - spacing.bottom;

    return result;
}

static inline Rectangle rectangle_expand(Rectangle rect, Insets spacing)
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

static inline Rectangle rectangle_top(Rectangle rect, int size)
{
    return (Rectangle){{rect.X, rect.Y, rect.width, size}};
}

static inline Rectangle rectangle_bottom(Rectangle rect, int size)
{
    return (Rectangle){{rect.X, rect.Y + rect.height - size, rect.width, size}};
}

static inline Rectangle rectangle_left(Rectangle rect, int size)
{
    return (Rectangle){{rect.X, rect.Y, size, rect.height}};
}

static inline Rectangle rectangle_right(Rectangle rect, int size)
{
    return (Rectangle){{rect.X + rect.width - size, rect.Y, size, rect.height}};
}

#define RECTANGLE_BORDER_TOP (1 << 0)
#define RECTANGLE_BORDER_BOTTOM (1 << 1)
#define RECTANGLE_BORDER_LEFT (1 << 2)
#define RECTANGLE_BORDER_RIGHT (1 << 3)

typedef unsigned RectangeBorder;

static inline RectangeBorder rectangle_inset_containe_point(Rectangle rect, Insets spacing, Point position)
{
    RectangeBorder borders = 0;

    if (rectangle_containe_point(rectangle_top(rect, spacing.top), position))
    {
        borders |= RECTANGLE_BORDER_TOP;
    }

    if (rectangle_containe_point(rectangle_bottom(rect, spacing.bottom), position))
    {
        borders |= RECTANGLE_BORDER_BOTTOM;
    }

    if (rectangle_containe_point(rectangle_left(rect, spacing.left), position))
    {
        borders |= RECTANGLE_BORDER_LEFT;
    }

    if (rectangle_containe_point(rectangle_right(rect, spacing.right), position))
    {
        borders |= RECTANGLE_BORDER_RIGHT;
    }

    return borders;
}

static inline Rectangle rectangle_center_within(Rectangle rectangle, Rectangle container)
{
    return (Rectangle){{
        container.X + container.width / 2 - rectangle.width / 2,
        container.Y + container.height / 2 - rectangle.height / 2,

        rectangle.width,
        rectangle.height,
    }};
}
