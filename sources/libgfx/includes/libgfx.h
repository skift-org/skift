#pragma once
#include "types.h"

typedef union 
{
    struct
    {
        uchar alpha;
        uchar red;
        uchar green;
        uchar blue;
    } argb;

    uint value;
} color_t;

typedef struct 
{
    uint width;
    uint height;

    uint* data;
} bitmap_t;

typedef struct 
{
    bitmap_t * bitmap;
} drawing_context_t;

bitmap_t * bitmap_alloc(uint width, uint height);
void bitmap_free(bitmap_t * bitmap);
void bitmap_resize(bitmap_t * bitmap, uint width, uint height);

void png_decode_file();
void png_decode_memory();
void png_encode_file();
void png_encode_memory();

