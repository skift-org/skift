#pragma once
#include "types.h"

typedef struct
{
    int shared;

    uint width;
    uint height;

    uint *buffer;
} bitmap_t;

bitmap_t *graphic_bitmap_shared(uint width, uint height, uint *buffer);
bitmap_t *graphic_bitmap_alloc(uint width, uint height);
void graphic_bitmap_free(bitmap_t *bmp);

void graphic_pixel(bitmap_t *bmp, uint x, uint y, uint color);
void graphic_line(bitmap_t *bmp);
void graphic_rectangle(bitmap_t*bmp);