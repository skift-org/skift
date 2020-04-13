#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/Color.h>
#include <libgraphic/Shape.h>
#include <libmath/math.h>
#include <libsystem/Result.h>

typedef enum
{
    BITMAP_FILTERING_NEAREST,
    BITMAP_FILTERING_LINEAR,
} BitmapFiltering;

typedef struct
{
    int width;
    int height;
    BitmapFiltering filtering;

    Color pixels[];
} Bitmap;

Bitmap *bitmap_create(size_t width, size_t height);

void bitmap_destroy(Bitmap *bitmap);

Rectangle bitmap_bound(Bitmap *bitmap);

Bitmap *bitmap_load_from(const char *path);

Result bitmap_load_from_can_fail(const char *path, Bitmap **bitmap);

Result bitmap_save_to(Bitmap *bitmap, const char *path);

static inline void bitmap_set_pixel(Bitmap *bitmap, Point p, Color color)
{
    if ((p.X >= 0 && p.X < bitmap->width) && (p.Y >= 0 && p.Y < bitmap->height))
        bitmap->pixels[(int)(p.X + p.Y * bitmap->width)] = color;
}

static inline void bitmap_set_pixel_no_check(Bitmap *bitmap, Point position, Color color)
{
    bitmap->pixels[(int)(position.X + position.Y * bitmap->width)] = color;
}

static inline Color bitmap_get_pixel(Bitmap *bitmap, Point p)
{
    int xi = abs((int)p.X % bitmap->width);
    int yi = abs((int)p.Y % bitmap->height);

    return bitmap->pixels[xi + yi * bitmap->width];
}

static inline Color bitmap_get_pixel_no_check(Bitmap *bitmap, Point position)
{
    return bitmap->pixels[position.X + position.Y * bitmap->width];
}

static inline Color bitmap_sample(Bitmap *bitmap, Rectangle src_rect, float x, float y)
{
    Color result;

    float xx = src_rect.width * x;
    float yy = src_rect.height * y;

    int xxi = (int)xx;
    int yyi = (int)yy;

    if (bitmap->filtering == BITMAP_FILTERING_NEAREST)
    {
        result = bitmap_get_pixel(bitmap, (Point){src_rect.X + xxi, src_rect.Y + yyi});
    }
    else
    {
        Color c00 = bitmap_get_pixel(bitmap, (Point){src_rect.X + xxi, src_rect.Y + yyi});
        Color c10 = bitmap_get_pixel(bitmap, (Point){src_rect.X + xxi + 1, src_rect.Y + yyi});
        Color c01 = bitmap_get_pixel(bitmap, (Point){src_rect.X + xxi, src_rect.Y + yyi + 1});
        Color c11 = bitmap_get_pixel(bitmap, (Point){src_rect.X + xxi + 1, src_rect.Y + yyi + 1});

        result = color_blerp(c00, c10, c01, c11, xx - xxi, yy - yyi);
    }

    return result;
}

static inline void bitmap_blend_pixel(Bitmap *bitmap, Point p, Color color)
{
    Color bg = bitmap_get_pixel(bitmap, p);
    bitmap_set_pixel(bitmap, p, color_blend(color, bg));
}

static inline void bitmap_blend_pixel_no_check(Bitmap *bitmap, Point p, Color color)
{
    Color bg = bitmap_get_pixel_no_check(bitmap, p);
    bitmap_set_pixel_no_check(bitmap, p, color_blend(color, bg));
}
