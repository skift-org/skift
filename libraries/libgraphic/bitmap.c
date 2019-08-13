/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/bitmap.h>
#include <libsystem/logger.h>

#include "lodepng.h"

void bitmap_delete(bitmap_t *bmp);

bitmap_t *bitmap_from_buffer(uint width, uint height, color_t *buffer)
{
    bitmap_t *bitmap = OBJECT(bitmap);

    bitmap->width = width;
    bitmap->height = height;
    bitmap->buffer = buffer;
    bitmap->shared = true;

    bitmap->filtering = BITMAP_FILTERING_NEAREST;

    return bitmap;
}

bitmap_t *bitmap(uint width, uint height)
{
    bitmap_t *bitmap = bitmap_from_buffer(width, height, (color_t *)malloc(sizeof(color_t) * width * height));

    bitmap->shared = false;

    return bitmap;
}

void bitmap_delete(bitmap_t *bmp)
{
    if (!bmp->shared)
        free(bmp->buffer);
}

void bitmap_set_pixel(bitmap_t *bmp, point_t p, color_t color)
{
    if ((p.X >= 0 && p.X < bmp->width) && (p.Y >= 0 && p.Y < bmp->height))
        bmp->buffer[(int)(p.X + p.Y * bmp->width)] = color;
}

color_t bitmap_get_pixel(bitmap_t *bmp, point_t p)
{
    int xi = abs((int)p.X % bmp->width);
    int yi = abs((int)p.Y % bmp->height);

    return bmp->buffer[xi + yi * bmp->width];
}

color_t bitmap_sample(bitmap_t *bmp, rectangle_t src_rect, float x, float y)
{
    color_t result;

    float xx = src_rect.width * x;
    float yy = src_rect.height * y;

    int xxi = (int)xx;
    int yyi = (int)yy;

    if (bmp->filtering == BITMAP_FILTERING_NEAREST)
    {
        result = bitmap_get_pixel(bmp, (point_t){src_rect.X + xxi, src_rect.Y + yyi});
    }
    else
    {
        color_t c00 = bitmap_get_pixel(bmp, (point_t){src_rect.X + xxi, src_rect.Y + yyi});
        color_t c10 = bitmap_get_pixel(bmp, (point_t){src_rect.X + xxi + 1, src_rect.Y + yyi});
        color_t c01 = bitmap_get_pixel(bmp, (point_t){src_rect.X + xxi, src_rect.Y + yyi + 1});
        color_t c11 = bitmap_get_pixel(bmp, (point_t){src_rect.X + xxi + 1, src_rect.Y + yyi + 1});

        result = color_blerp(c00, c10, c01, c11, xx - xxi, yy - yyi);
    }

    return result;
}

rectangle_t bitmap_bound(bitmap_t *bmp)
{
    return (rectangle_t){{0, 0, bmp->width, bmp->height}};
}

void bitmap_blend_pixel(bitmap_t *bmp, point_t p, color_t color)
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

bitmap_t *bitmap_load_from(const char *path)
{
    uchar *buffer;
    uint width = 0, height = 0;
    int error = 0;

    if ((error = lodepng_decode32_file(&buffer, &width, &height, path)) == 0)
    {
        bitmap_t *bmp = bitmap_from_buffer(width, height, (color_t *)buffer);
        bmp->shared = false;
        return bmp;
    }
    else
    {
        logger_log(LOG_ERROR, "lodepng: failled to load %s: %s", path, lodepng_error_text(error));
        return bitmap_from_buffer(2, 2, (color_t *)&placeholder_buffer);
    }
}

int bitmap_save_to(bitmap_t *bmp, const char *path)
{
    return lodepng_encode32_file(path, (const uchar *)bmp->buffer, bmp->width, bmp->height);
}