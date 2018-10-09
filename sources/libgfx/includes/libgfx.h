#pragma once

#include "types.h"
#include "utils.h"

typedef struct
{
    bool shared;

    int width;
    int height;

    uint *buffer;
} bitmap_t;

bitmap_t *bitmap_ctor(uint width, uint height);
void bitmap_dtor(bitmap_t *bmp);

void libgfx_clear(bitmap_t *bmp, uint color);
void libgfx_pixel(bitmap_t *bmp, int x, int y, uint color);
void libgfx_line(bitmap_t *bmp, int x0, int y0, int x1, int y1, int weight, uint color);
void libgfx_rect(bitmap_t *bmp, int x, int y, int w, int h, int weight, uint color);
void libgfx_fillrect(bitmap_t *bmp, int x, int y, int w, int h, uint color);

void libgfx_char(bitmap_t *bmp, int x, int y, char c);
void libgfx_text(bitmap_t *bmp, int x, int y, const char *str);