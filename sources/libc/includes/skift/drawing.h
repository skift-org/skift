#pragma once
#include <stdbool.h>
#include <skift/types.h>
#include <skift/utils.h>

typedef struct
{
    bool shared;

    int width;
    int height;

    uint *buffer;
} bitmap_t;

bitmap_t *bitmap_ctor(uint width, uint height);
void bitmap_dtor(bitmap_t *bmp);

void drawing_clear(bitmap_t *bmp, uint color);
void drawing_pixel(bitmap_t *bmp, int x, int y, uint color);
void drawing_line(bitmap_t *bmp, int x0, int y0, int x1, int y1, int weight, uint color);

void drawing_rect(bitmap_t *bmp, int x, int y, int w, int h, int weight, uint color);
void drawing_fillrect(bitmap_t *bmp, int x, int y, int w, int h, uint color);

void drawing_filltri(bitmap_t * bmp, int x0, int y0, int x1, int y1, int x2, int y2, uint color);

void drawing_char(bitmap_t *bmp, char c, int x, int y, uint color);
void drawing_text(bitmap_t *bmp, const char * str, int x, int y, uint color);