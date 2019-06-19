#pragma once

#include <skift/list.h>
#include <skift/math.h>
#include <skift/colors.h>

typedef struct
{
    int X;
    int Y;
} point_t;

#define point_zero ((point_t){0, 0})

static inline point_t point_clamp(point_t p, point_t pmin, point_t pmax)
{
    p.X = max(pmin.X, min(pmax.X, p.X));
    p.Y = max(pmin.Y, min(pmax.Y, p.Y));

    return p;
}

static inline point_t point_add(point_t a, point_t b)
{
    return (point_t){a.X + b.X, a.Y + b.Y};
}

static inline point_t point_sub(point_t a, point_t b)
{
    return (point_t){a.X - b.X, a.Y - b.Y};
}

static inline point_t point_x(point_t p)
{
    return (point_t){.X = p.X, .Y = 0};
}

static inline point_t point_y(point_t p)
{
    return (point_t){.X = 0, .Y = p.Y};
}

typedef union {
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
    return (rectange.X <= position.X && (rectange.X + rectange.width)  > position.X) &&
           (rectange.Y <= position.Y && (rectange.Y + rectange.height) > position.Y);
}

/* --- Bitmap object -------------------------------------------------------- */

typedef enum
{
    BITMAP_FILTERING_NEAREST,
    BITMAP_FILTERING_LINEAR,
} bitmap_filtering_t;

typedef struct
{
    bool shared;

    int width;
    int height;

    bitmap_filtering_t filtering;

    color_t *buffer;
} bitmap_t;

bitmap_t *bitmap(uint width, uint height);

void bitmap_delete(bitmap_t *bmp);

bitmap_t *bitmap_load_from(const char *path);

int bitmap_save_to(bitmap_t *bmp, const char *path);

void bitmap_set_pixel(bitmap_t *bmp, point_t p, color_t color);

color_t bitmap_get_pixel(bitmap_t *bmp, point_t p);

color_t bitmap_sample(bitmap_t *bmp, rectangle_t src_rect, float x, float y);

void bitmap_blend_pixel(bitmap_t *bmp, point_t p, color_t color);

rectangle_t bitmap_bound(bitmap_t *bmp);

/* --- Painter -------------------------------------------------------------- */

typedef struct
{
    bitmap_t *bitmap;

    rectangle_t cliprect;
    int cliprect_stack_top;
    rectangle_t cliprect_stack[32];
} painter_t;

painter_t *painter(bitmap_t *bmp);

void painter_delete(painter_t *paint);

void painter_push_cliprect(painter_t *paint, rectangle_t cliprect);

void painter_pop_cliprect(painter_t *paint);

void painter_plot_pixel(painter_t *painter, point_t position, color_t color);

void painter_blit_bitmap(painter_t *paint, bitmap_t *src, rectangle_t src_rect, rectangle_t dst_rect);

void painter_fill_rect(painter_t *paint, rectangle_t rect, color_t color);

// TODO: void painter_fill_circle(painter_t* paint, ...);

// TODO: void painter_fill_triangle(painter_t* paint, ...);

void painter_draw_line(painter_t *paint, point_t a, point_t b, color_t color);

void painter_draw_rect(painter_t *paint, rectangle_t rect, color_t color);

// TODO: void painter_draw_circle(painter_t* paint, ...);

// TODO: void painter_draw_triangle(painter_t* paint, ...);

void painter_draw_text(painter_t *paint, const char *text, point_t position, color_t color);
