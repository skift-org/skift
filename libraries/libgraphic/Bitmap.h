#pragma once

#include <libgraphic/Color.h>
#include <libgraphic/Shape.h>
#include <libsystem/Result.h>
#include <libsystem/math/Math.h>

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

static inline void bitmap_set_pixel(Bitmap *bitmap, Vec2i p, Color color)
{
    if (bitmap_bound(bitmap).containe(p))
        bitmap->pixels[(int)(p.x() + p.y() * bitmap->width)] = color;
}

static inline void bitmap_set_pixel_no_check(Bitmap *bitmap, Vec2i position, Color color)
{
    bitmap->pixels[(int)(position.x() + position.y() * bitmap->width)] = color;
}

static inline Color bitmap_get_pixel(Bitmap *bitmap, Vec2i position)
{
    int xi = abs(position.x() % bitmap->width);
    int yi = abs(position.y() % bitmap->height);

    return bitmap->pixels[xi + yi * bitmap->width];
}

static inline Color bitmap_get_pixel_no_check(Bitmap *bitmap, Vec2i position)
{
    return bitmap->pixels[position.x() + position.y() * bitmap->width];
}

static inline Color bitmap_sample(Bitmap *bitmap, Rectangle src_rect, double x, double y)
{
    Color result;

    double xx = src_rect.width() * x;
    double yy = src_rect.height() * y;

    int xxi = (int)xx;
    int yyi = (int)yy;

    if (bitmap->filtering == BITMAP_FILTERING_NEAREST)
    {
        result = bitmap_get_pixel(bitmap, Vec2i(src_rect.x() + xxi, src_rect.y() + yyi));
    }
    else
    {
        Color c00 = bitmap_get_pixel(bitmap, Vec2i(src_rect.x() + xxi, src_rect.y() + yyi));
        Color c10 = bitmap_get_pixel(bitmap, Vec2i(src_rect.x() + xxi + 1, src_rect.y() + yyi));
        Color c01 = bitmap_get_pixel(bitmap, Vec2i(src_rect.x() + xxi, src_rect.y() + yyi + 1));
        Color c11 = bitmap_get_pixel(bitmap, Vec2i(src_rect.x() + xxi + 1, src_rect.y() + yyi + 1));

        result = color_blerp(c00, c10, c01, c11, xx - xxi, yy - yyi);
    }

    return result;
}

static inline void bitmap_blend_pixel(Bitmap *bitmap, Vec2i p, Color color)
{
    Color bg = bitmap_get_pixel(bitmap, p);
    bitmap_set_pixel(bitmap, p, color_blend(color, bg));
}

static inline void bitmap_blend_pixel_no_check(Bitmap *bitmap, Vec2i p, Color color)
{
    Color bg = bitmap_get_pixel_no_check(bitmap, p);
    bitmap_set_pixel_no_check(bitmap, p, color_blend(color, bg));
}

void bitmap_copy(Bitmap *source, Bitmap *destination, Rectangle region);
