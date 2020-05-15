#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libmath/MinMax.h>
#include <libmath/Vectors.h>
#include <libmath/math.h>

typedef struct __packed
{
    int x;
    int y;
} Point;

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
        int x;
        int y;
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
    return (Point){a.x + b.x, a.y + b.y};
}

static inline Point point_sub(Point a, Point b)
{
    return (Point){a.x - b.x, a.y - b.y};
}

static inline Point point_div(Point a, int value)
{
    return (Point){a.x / value, a.y / value};
}

static inline bool point_equ(Point lhs, Point rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

static inline Point point_scale(Point a, double scale)
{
    return (Point){(int)(a.x * scale), (int)(a.y * scale)};
}

static inline Point point_x(Point p)
{
    return (Point){p.x, 0};
}

static inline Point point_y(Point p)
{
    return (Point){0, p.y};
}

static inline Point point_clamp(Point p, Point pmin, Point pmax)
{
    p.x = MAX(pmin.x, MIN(pmax.x, p.x));
    p.y = MAX(pmin.y, MIN(pmax.y, p.y));

    return p;
}

static inline Point point_clamp_to_rect(Point p, Rectangle rect)
{
    return point_clamp(p, rect.position, point_add(rect.position, rect.size));
}

static inline Rectangle rectangle_min_size(Rectangle rectangle, Point size)
{
    rectangle.width = MIN(size.x, rectangle.width);
    rectangle.width = MIN(size.y, rectangle.height);

    return rectangle;
}

static inline Rectangle rectangle_max_size(Rectangle rectangle, Point size)
{
    rectangle.width = MAX(size.x, rectangle.width);
    rectangle.width = MAX(size.y, rectangle.height);

    return rectangle;
}

static inline Rectangle rectangle_from_two_point(Point a, Point b)
{
    return (Rectangle){{
        MIN(a.x, b.x),
        MIN(a.y, b.y),
        abs(a.x - b.x),
        abs(a.y - b.y),
    }};
}

static inline bool rectangle_colide(Rectangle a, Rectangle b)
{
    return a.x < b.x + b.width &&
           a.x + a.width > b.x &&
           a.y < b.y + b.height &&
           a.height + a.y > b.y;
}

static inline int rectangle_area(Rectangle rectangle)
{
    return rectangle.width * rectangle.height;
}

static inline Rectangle rectangle_merge(Rectangle a, Rectangle b)
{
    Point topleft;
    topleft.x = MIN(a.x, b.x);
    topleft.y = MIN(a.y, b.y);

    Point bottomright;
    bottomright.x = MAX(a.x + a.width, b.x + b.width);
    bottomright.y = MAX(a.y + a.height, b.y + b.height);

    Rectangle rectangle = {};

    rectangle.position = topleft;
    rectangle.size = point_sub(bottomright, topleft);

    return rectangle;
}

static inline Rectangle rectangle_clip(Rectangle left, Rectangle right)
{
    int x = MAX(left.x, right.x);
    int y = MAX(left.y, right.y);

    int width = MIN(left.x + left.width, right.x + right.width) - x;
    int height = MIN(left.y + left.height, right.y + right.height) - y;

    return (Rectangle){{x, y, width, height}};
}

static inline bool rectangle_containe_point(Rectangle rectange, Point position)
{
    return (rectange.x <= position.x && (rectange.x + rectange.width) > position.x) &&
           (rectange.y <= position.y && (rectange.y + rectange.height) > position.y);
}

static inline bool rectangle_container_rectangle(Rectangle rectange, Rectangle contained)
{
    return (rectange.x <= contained.x && (contained.x + contained.width) >= contained.x) &&
           (rectange.y <= contained.y && (contained.y + contained.height) >= contained.y);
}

static inline Rectangle rectangle_shrink(Rectangle rect, Insets spacing)
{
    Rectangle result;

    result.x = rect.x + spacing.left;
    result.y = rect.y + spacing.top;
    result.width = rect.width - spacing.left - spacing.right;
    result.height = rect.height - spacing.top - spacing.bottom;

    return result;
}

static inline Rectangle rectangle_expand(Rectangle rect, Insets spacing)
{
    Rectangle result;

    result.x = rect.x - spacing.left;
    result.y = rect.y - spacing.top;
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
    return (Rectangle){{rect.x, rect.y, rect.width, size}};
}

static inline Rectangle rectangle_bottom(Rectangle rect, int size)
{
    return (Rectangle){{rect.x, rect.y + rect.height - size, rect.width, size}};
}

static inline Rectangle rectangle_left(Rectangle rect, int size)
{
    return (Rectangle){{rect.x, rect.y, size, rect.height}};
}

static inline Rectangle rectangle_right(Rectangle rect, int size)
{
    return (Rectangle){{rect.x + rect.width - size, rect.y, size, rect.height}};
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
        container.x + container.width / 2 - rectangle.width / 2,
        container.y + container.height / 2 - rectangle.height / 2,

        rectangle.width,
        rectangle.height,
    }};
}
