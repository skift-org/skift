/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/bitmap.h>
#include <libsystem/logger.h>

#include "lodepng.h"

Bitmap *bitmap_create(uint width, uint height)
{
    Bitmap *this = malloc(sizeof(Bitmap) + width * height * sizeof(color_t));

    this->width = width;
    this->height = height;
    this->filtering = BITMAP_FILTERING_NEAREST;

    return this;
}

void bitmap_destroy(Bitmap *this)
{
    free(this);
}

void bitmap_set_pixel(Bitmap *bmp, Point p, color_t color)
{
    if ((p.X >= 0 && p.X < bmp->width) && (p.Y >= 0 && p.Y < bmp->height))
        bmp->pixels[(int)(p.X + p.Y * bmp->width)] = color;
}

color_t bitmap_get_pixel(Bitmap *bmp, Point p)
{
    int xi = abs((int)p.X % bmp->width);
    int yi = abs((int)p.Y % bmp->height);

    return bmp->pixels[xi + yi * bmp->width];
}

color_t bitmap_sample(Bitmap *bmp, Rectangle src_rect, float x, float y)
{
    color_t result;

    float xx = src_rect.width * x;
    float yy = src_rect.height * y;

    int xxi = (int)xx;
    int yyi = (int)yy;

    if (bmp->filtering == BITMAP_FILTERING_NEAREST)
    {
        result = bitmap_get_pixel(bmp, (Point){src_rect.X + xxi, src_rect.Y + yyi});
    }
    else
    {
        color_t c00 = bitmap_get_pixel(bmp, (Point){src_rect.X + xxi, src_rect.Y + yyi});
        color_t c10 = bitmap_get_pixel(bmp, (Point){src_rect.X + xxi + 1, src_rect.Y + yyi});
        color_t c01 = bitmap_get_pixel(bmp, (Point){src_rect.X + xxi, src_rect.Y + yyi + 1});
        color_t c11 = bitmap_get_pixel(bmp, (Point){src_rect.X + xxi + 1, src_rect.Y + yyi + 1});

        result = color_blerp(c00, c10, c01, c11, xx - xxi, yy - yyi);
    }

    return result;
}

Rectangle bitmap_bound(Bitmap *bmp)
{
    return (Rectangle){{0, 0, bmp->width, bmp->height}};
}

void bitmap_blend_pixel(Bitmap *bmp, Point p, color_t color)
{
    color_t bg = bitmap_get_pixel(bmp, p);
    bitmap_set_pixel(bmp, p, color_blend(color, bg));
}

static color_t placeholder_buffer[] = {
    (color_t){{255, 0, 255, 255}},
    (color_t){{0, 0, 0, 255}},
    (color_t){{0, 0, 0, 255}},
    (color_t){{255, 0, 255, 255}},
};

Bitmap *bitmap_load_from(const char *path)
{
    uchar *buffer;
    uint width = 0, height = 0;
    int error = 0;

    if ((error = lodepng_decode32_file(&buffer, &width, &height, path)) == 0)
    {
        Bitmap *this = bitmap_create(width, height);

        memcpy(this->pixels, buffer, width * height * sizeof(color_t));
        free(buffer);

        return this;
    }
    else
    {
        logger_error("Failled to load from %s: %s", path, lodepng_error_text(error));

        Bitmap *this = bitmap_create(width, height);
        memcpy(this->pixels, placeholder_buffer, 2 * 2 * sizeof(color_t));
        return this;
    }
}

int bitmap_save_to(Bitmap *bmp, const char *path)
{
    return lodepng_encode32_file(path, (const uchar *)bmp->pixels, bmp->width, bmp->height);
}