#pragma once
#include <stdbool.h>
#include <skift/types.h>
#include <skift/utils.h>

/* --- Colors --------------------------------------------------------------- */

typedef unsigned int color_t;
#define rbg(r, g, b) (color_t)((r) & 0xff) | ((g) & 0xff) | ((b) & 0xff)

/* --- Bitmap --------------------------------------------------------------- */

typedef struct
{
    bool shared;

    int width;
    int height;

    uint *buffer;
} bitmap_t;

bitmap_t *bitmap(uint width, uint height);
void bitmap_delete(bitmap_t *bmp);

/* --- Drawing -------------------------------------------------------------- */

typedef struct 
{
    bitmap_t * surface;
    color_t color;
} drawing_context_t;

void drawing_begin(drawing_context_t * ctx, bitmap_t * surface);
void drawing_end(drawing_context_t * ctx);

void drawing_clear(bitmap_t *bmp, uint color);

void drawing_pixel(bitmap_t *bmp, int x, int y, uint color);
void drawing_line(bitmap_t *bmp, int x0, int y0, int x1, int y1, int weight, uint color);
void drawing_fill(bitmap_t * bmp, int x, int y, uint color);

void drawing_rect(bitmap_t *bmp, int x, int y, int w, int h, int weight, uint color);
void drawing_fillrect(bitmap_t *bmp, int x, int y, int w, int h, uint color);

void drawing_tri(bitmap_t *bmp, int x0, int y0, int x1, int y1, int x2, int y2, uint color);
void drawing_filltri(bitmap_t *bmp, int x0, int y0, int x1, int y1, int x2, int y2, uint color);

void drawing_char(bitmap_t *bmp, char c, int x, int y, uint color);
void drawing_text(bitmap_t *bmp, const char *str, int x, int y, uint color);