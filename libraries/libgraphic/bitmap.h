#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/color.h>
#include <libgraphic/shape.h>
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

void bitmap_destroy(Bitmap *this);

Bitmap *bitmap_load_from(const char *path);

Result bitmap_save_to(Bitmap *bmp, const char *path);

void bitmap_set_pixel(Bitmap *bmp, Point p, Color color);

Color bitmap_get_pixel(Bitmap *bmp, Point p);

Color bitmap_sample(Bitmap *bmp, Rectangle src_rect, float x, float y);

void bitmap_blend_pixel(Bitmap *bmp, Point p, Color color);

Rectangle bitmap_bound(Bitmap *bmp);
