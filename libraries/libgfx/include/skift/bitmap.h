#pragma once

#include <skift/math.h>
#include <skift/colors.h>
#include <skift/shape.h>

typedef enum
{
    BITMAP_FILTERING_NEAREST,
    BITMAP_FILTERING_LINEAR,
} bitmap_filtering_t;

typedef struct bitmap_s
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
