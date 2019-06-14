#include <skift/assert.h>
#include <skift/drawing.h>

#include "lodepng.h"
#include "vgafont.h"

/* --- Bitmap --------------------------------------------------------------- */

bitmap_t *bitmap_from_buffer(uint width, uint height, color_t *buffer)
{
    bitmap_t *bitmap = MALLOC(bitmap_t);

    bitmap->width = width;
    bitmap->height = height;
    bitmap->buffer = buffer;
    bitmap->shared = true;

    return bitmap;
}

bitmap_t *bitmap(uint width, uint height)
{
    bitmap_t *bitmap = bitmap_from_buffer(width, height, (color_t*)malloc(sizeof(color_t) * width * height));

    bitmap->shared = false;

    return bitmap;
}

void bitmap_delete(bitmap_t *bmp)
{
    if (!bmp->shared)
        free(bmp->buffer);

    free(bmp);
}

void bitmap_set_pixel(bitmap_t *bmp, point_t p, color_t color)
{
    if ((p.X >= 0 && p.X < bmp->width) && (p.Y >= 0 && p.Y < bmp->height))
        bmp->buffer[p.X + p.Y * bmp->width] = color;
}

color_t bitmap_get_pixel(bitmap_t *bmp, point_t p)
{
    if ((p.X >= 0 && p.X < bmp->width) && (p.Y >= 0 && p.Y < bmp->height))
    {
        return bmp->buffer[p.X + p.Y * bmp->width];
    }
    else
    {
        return (color_t){{255, 0, 255, 255}};
    }
}

color_t bitmap_sample(bitmap_t* bmp, rectangle_t src_rect, float x, float y)
{
    int xx = (int)(src_rect.width  * x) % src_rect.width;
    int yy = (int)(src_rect.height * y) % src_rect.height;

    // FIXME: maybe do some kind of filtering here ?

    return bitmap_get_pixel(bmp, (point_t){src_rect.X + xx, src_rect.Y + yy});
}

rectangle_t bitmap_bound(bitmap_t *bmp)
{
    return (rectangle_t){{0, 0, bmp->width, bmp->height}};
}

void bitmap_blend_pixel(bitmap_t *bmp, point_t p, color_t color)
{
    color_t bg = bitmap_get_pixel(bmp, p);
    bitmap_set_pixel(bmp, p, color_blend(color, bg));
}

#include <skift/logger.h>

bitmap_t *bitmap_load_from(const char *path)
{
    uchar *buffer;
    uint width = 0, height = 0;
    int error = 0;

    if ((error = lodepng_decode32_file(&buffer, &width, &height, path)) == 0)
    {
        bitmap_t* bmp = bitmap_from_buffer(width, height, (color_t*)buffer);
        bmp->shared = false;
        return bmp;
    }
    else
    {
        logger_log(LOG_ERROR, "lodepng: failled to load %s: %s", path, lodepng_error_text(error));
        return NULL;
    }
}

int bitmap_save_to(bitmap_t *bmp, const char *path)
{
    return lodepng_encode32_file(path, (const uchar*)bmp->buffer, bmp->width, bmp->height);
}

/* --- Painter -------------------------------------------------------------- */

painter_t *painter(bitmap_t *bmp)
{
    painter_t *paint = MALLOC(painter_t);

    paint->bitmap = bmp;
    paint->cliprect = bitmap_bound(bmp);
    paint->cliprect_stack_top = 0;

    return paint;
}

void painter_delete(painter_t *paint)
{
    free(paint);
}

void painter_push_cliprect(painter_t *paint, rectangle_t cliprect)
{
    assert(paint->cliprect_stack_top < 32);

    paint->cliprect_stack[paint->cliprect_stack_top] = paint->cliprect;
    paint->cliprect_stack_top++;

    paint->cliprect = rectangle_child(paint->cliprect, cliprect);
}

void painter_pop_cliprect(painter_t *paint)
{
    assert(paint->cliprect_stack_top > 0);

    paint->cliprect_stack_top--;
    paint->cliprect = paint->cliprect_stack[paint->cliprect_stack_top];
}

void painter_plot_pixel(painter_t *painter, point_t position, color_t color)
{
    point_t point_absolue = {painter->cliprect.X + position.X, painter->cliprect.Y + position.Y};

    if (rectangle_containe_position(painter->cliprect, point_absolue))
    {
        bitmap_blend_pixel(painter->bitmap, point_absolue, color);
    }
}

void painter_blit_bitmap(painter_t *paint, bitmap_t *src, rectangle_t src_rect, rectangle_t dst_rect)
{
    for (int x = 0; x < dst_rect.width; x++)
    {
        for (int y = 0; y < dst_rect.height; y++)
        {
            color_t pix = bitmap_get_pixel(src, point_add(src_rect.position, (point_t){x, y}));
            painter_plot_pixel(paint, point_add(dst_rect.position, (point_t){x, y}), pix);
        }
    }
}

void painter_fill_rect(painter_t *paint, rectangle_t rect, color_t color)
{
    rectangle_t rect_absolue = rectangle_child(paint->cliprect, rect);

    for (int xx = 0; xx < rect_absolue.width; xx++)
    {
        for (int yy = 0; yy < rect_absolue.height; yy++)
        {
            bitmap_blend_pixel(
                paint->bitmap,
                (point_t){rect_absolue.X + xx, rect_absolue.Y + yy},
                color);
        }
    }
}

// TODO: void painter_fill_circle(painter_t* paint, ...);

// TODO: void painter_fill_triangle(painter_t* paint, ...);

void painter_draw_line_x_aligned(painter_t *paint, int x, int start, int end, color_t color)
{
    for (int i = start; i < end; i++)
    {
        painter_plot_pixel(paint, (point_t){x, i}, color);
    }
}

void painter_draw_line_y_aligned(painter_t *paint, int y, int start, int end, color_t color)
{
    for (int i = start; i < end; i++)
    {
        painter_plot_pixel(paint, (point_t){i, y}, color);
    }
}

void painter_draw_line_not_aligned(painter_t *paint, point_t a, point_t b, color_t color)
{
    int dx = abs(b.X - a.X), sx = a.X < b.X ? 1 : -1;
    int dy = abs(b.Y - a.Y), sy = a.Y < b.Y ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        painter_plot_pixel(paint, a, color);

        if (a.X == b.X && a.Y == b.Y)
            break;

        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            a.X += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            a.Y += sy;
        }
    }
}

void painter_draw_line(painter_t *paint, point_t a, point_t b, color_t color)
{
    if (a.X == b.X)
    {
        painter_draw_line_x_aligned(paint, a.X, min(a.Y, b.Y), max(a.Y, b.Y), color);
    }
    else if (a.Y == b.Y)
    {
        painter_draw_line_y_aligned(paint, a.Y, min(a.X, b.X), max(a.X, b.X), color);
    }
    else
    {
        painter_draw_line_not_aligned(paint, a, b, color);
    }
}

void painter_draw_rect(painter_t *paint, rectangle_t rect, color_t color)
{
    painter_draw_line(paint, rect.position, point_add(rect.position, point_x(rect.size)), color);
    painter_draw_line(paint, rect.position, point_add(rect.position, point_y(rect.size)), color);
    painter_draw_line(paint, point_add(rect.position, point_x(rect.size)), point_add(rect.position, rect.size), color);
    painter_draw_line(paint, point_add(rect.position, point_y(rect.size)), point_add(rect.position, rect.size), color);
}

// TODO: void painter_draw_circle(painter_t* paint, ...);

// TODO: void painter_draw_triangle(painter_t* paint, ...);

static const int glyph_mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};

void painter_draw_glyph(painter_t *paint, int chr, point_t position, color_t color)
{
    uchar *gylph = vgafont16 + chr * 16;

    for (int cy = 0; cy < 16; cy++)
    {
        for (int cx = 0; cx < 8; cx++)
        {
            if (gylph[cy] & glyph_mask[cx])
            {
                painter_plot_pixel(paint, point_add(position, (point_t){cx, cy}), color);
            }
        }
    }
}

void painter_draw_text(painter_t *paint, const char *text, point_t position, color_t color)
{
    for (size_t i = 0; text[i]; i++)
    {
        painter_draw_glyph(paint, text[i], point_add(position, (point_t){i*9, 0}), color);
    }
}
