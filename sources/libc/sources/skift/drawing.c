#include <stdlib.h>
#include <skift/drawing.h>

#define BMP_SIZE_MEM(bmp) (bmp->width * bmp->height * sizeof(uint))
#define BMP_SIZE(bmp) (uint)(bmp->width * bmp->height)

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

void drawing_pixel(bitmap_t *bmp, int x, int y, uint color)
{
    if (x >= 0 && x < bmp->width && y >= 0 && y < bmp->height)
        bmp->buffer[x + y * bmp->width] = color;
}

inline void drawing_pixel_inline(bitmap_t *bmp, int x, int y, uint color)
{
    if (x >= 0 && x < bmp->width && y >= 0 && y < bmp->height)
        bmp->buffer[x + y * bmp->width] = color;
}

void drawing_clear(bitmap_t *bmp, uint color)
{
    for (size_t i = 0; i < BMP_SIZE(bmp); i++)
        bmp->buffer[i] = color;
}

void drawing_line(bitmap_t *bmp, int x0, int y0, int x1, int y1, int weight, uint color)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        for (int xoff = -weight + 1; xoff < weight; xoff++)
            for (int yoff = -weight + 1; yoff < weight; yoff++)
                drawing_pixel_inline(bmp, x0 + xoff, y0 + yoff, color);

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

void drawing_rect(bitmap_t *bmp, int x, int y, int w, int h, int weight, uint color)
{

    // + A +
    // B   D
    // + C +

    drawing_line(bmp, x, y, x + w, y, weight, color); // A
    drawing_line(bmp, x, y, x, y + h, weight, color); // B

    drawing_line(bmp, x + w, y, x + w, y + h, weight, color); // D
    drawing_line(bmp, x, y + h, x + w, y + h, weight, color); // C
}

void drawing_fillrect(bitmap_t *bmp, int x, int y, int w, int h, uint color)
{
    for (int xx = 0; xx < w; xx++)
        for (int yy = 0; yy < h; yy++)
            drawing_pixel_inline(bmp, x + xx, y + yy, color);
            //bmp->buffer[(x + xx) + (y + yy) * bmp->width] = color;
}

void drawing_filltri(bitmap_t * bmp, int x0, int y0, int x1, int y1, int x2, int y2, uint color)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        drawing_line(bmp, x2, y2, x0, y0 , 1, color);

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

int mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};

extern unsigned char vgafont16[256 * 16];

void drawing_char(bitmap_t *bmp, char c, int x, int y, uint color)
{
    uchar *gylph = vgafont16 + (int)c * 16;

    for (int cy = 0; cy < 16; cy++)
    {
        for (int cx = 0; cx < 8; cx++)
        {
            if (gylph[cy] & mask[cx])
            {
                drawing_pixel_inline(bmp, x + cx, y + cy, color);
            }
        }
    }
}

void drawing_text(bitmap_t *bmp, const char * str, int x, int y, uint color)
{
    char c;

    for (size_t i = 0; (c = str[i]); i++)
    {
        drawing_char(bmp, c, x + i * 9, y, color);
    }
}