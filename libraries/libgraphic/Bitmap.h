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

Result bitmap_save_to(Bitmap *bitmap, const char *path);

void bitmap_set_pixel(Bitmap *bitmap, Point position, Color color);

Color bitmap_get_pixel(Bitmap *bitmap, Point position);

Color bitmap_sample(Bitmap *bitmap, Rectangle source, float x, float y);

void bitmap_blend_pixel(Bitmap *bitmap, Point position, Color color);
