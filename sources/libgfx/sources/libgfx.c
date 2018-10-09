#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <libgfx.h>

#define BMP_SIZE(bmp) (bmp->width * bmp->height * sizeof(uint))

/* --- Bitmap --------------------------------------------------------------- */

bitmap_t *bitmap_ctor(uint width, uint height)
{
    bitmap_t *bmp = MALLOC(bitmap_t);
    bmp->buffer = (uint *)malloc(width * height * sizeof(uint));
    
    bmp->shared = 0;
    bmp->width = width;
    bmp->height = height;

    return bmp;
}

bitmap_t *bitmap_from_buffer(uint width, uint height, uint * buffer)
{
    bitmap_t *bmp = MALLOC(bitmap_t);
    bmp->buffer = buffer;

    bmp->shared = 1;
    bmp->width = width;
    bmp->height = height;

    return bmp;
}

void bitmap_dtor(bitmap_t *bmp)
{
    if (!bmp->shared) free(bmp->buffer);
    free(bmp);
}

/* --- Graphic -------------------------------------------------------------- */

void libgfx_clear(bitmap_t *bmp, uint color)
{
    for (size_t i = 0; i < BMP_SIZE(bmp); i++)
        bmp->buffer[i] = color;
}

void libgfx_pixel(bitmap_t *bmp, int x, int y, uint color)
{
    if (x >= 0 && x < bmp->width && y >= 0 && y < bmp->height)
        bmp->buffer[x + y * bmp->width] = color;
}

void libgfx_line(bitmap_t *bmp, int x0, int y0, int x1, int y1, int weight, uint color)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        for (int xoff = -weight + 1; xoff < weight; xoff++)
            for (int yoff = -weight + 1; yoff < weight; yoff++)
                libgfx_pixel(bmp, x0 + xoff, y0 + yoff, color);

        if (x0 == x1 && y0 == y1)
            break;

        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void libgfx_rect(bitmap_t *bmp, int x, int y, int w, int h, int weight, uint color)
{

    // + A +
    // B   D
    // + C +

    libgfx_line(bmp, x, y, x + w, y, weight, color); // A
    libgfx_line(bmp, x, y, x, y + h, weight, color); // B

    libgfx_line(bmp, x + w, y, x + w, y + h, weight, color); // D
    libgfx_line(bmp, x, y + h, x + w, y + h, weight, color); // C
}

void libgfx_fillrect(bitmap_t *bmp, int x, int y, int w, int h, uint color)
{
    for (int xx = 0; xx < w; xx++)
        for (int yy = 0; yy < h; yy++)
            libgfx_pixel(bmp, x + xx, y + yy, color);
}

int mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};

extern unsigned char vgafont16[256 * 16];

void libgfx_char(bitmap_t *bmp, int x, int y, char c)
{
    uchar *gylph = vgafont16 + (int)c * 16;

    for (int cy = 0; cy < 16; cy++)
        for (int cx = 0; cx < 8; cx++)
            libgfx_pixel(bmp, x + cx, y + cy, gylph[cy] & mask[cx] ? 0xffffff : 0x0);
}

void libgfx_text(bitmap_t *bmp, int x, int y, const char *str)
{
    char c;

    for (size_t i = 0; (c = str[i]); i++)
        libgfx_char(bmp, x + i, y, c);
}