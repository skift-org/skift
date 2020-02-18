/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/font.h>
#include <libgraphic/painter.h>
#include <libmath/math.h>
#include <libsystem/assert.h>

void painter_delete(Painter *);

Painter *painter_create(Bitmap *bitmap)
{
    Painter *paint = __create(Painter);

    paint->bitmap = bitmap;
    paint->cliprect = bitmap_bound(bitmap);
    paint->cliprect_stack_top = 0;

    return paint;
}

void painter_destroy(Painter *this)
{
    free(this);
}

void painter_push_cliprect(Painter *paint, Rectangle cliprect)
{
    assert(paint->cliprect_stack_top < 32);

    paint->cliprect_stack[paint->cliprect_stack_top] = paint->cliprect;
    paint->cliprect_stack_top++;

    paint->cliprect = rectangle_clip(paint->cliprect, cliprect);
}

void painter_pop_cliprect(Painter *paint)
{
    assert(paint->cliprect_stack_top > 0);

    paint->cliprect_stack_top--;
    paint->cliprect = paint->cliprect_stack[paint->cliprect_stack_top];
}

void painter_plot_pixel(Painter *painter, Point position, Color color)
{
    Point point_absolue = {painter->cliprect.X + position.X, painter->cliprect.Y + position.Y};

    if (rectangle_containe_point(painter->cliprect, point_absolue))
    {
        bitmap_blend_pixel(painter->bitmap, point_absolue, color);
    }
}

void painter_blit_bitmap_fast(Painter *paint, Bitmap *src, Rectangle src_rect, Rectangle dst_rect)
{
    for (int x = 0; x < dst_rect.width; x++)
    {
        for (int y = 0; y < dst_rect.height; y++)
        {
            Color pix = bitmap_get_pixel(src, (Point){src_rect.X + x, src_rect.Y + y});
            painter_plot_pixel(paint, point_add(dst_rect.position, (Point){x, y}), pix);
        }
    }
}

void painter_blit_bitmap_scaled(Painter *paint, Bitmap *src, Rectangle src_rect, Rectangle dst_rect)
{
    for (int x = 0; x < dst_rect.width; x++)
    {
        for (int y = 0; y < dst_rect.height; y++)
        {
            float xx = x / (float)dst_rect.width;
            float yy = y / (float)dst_rect.height;

            Color pix = bitmap_sample(src, src_rect, xx, yy);
            painter_plot_pixel(paint, point_add(dst_rect.position, (Point){x, y}), pix);
        }
    }
}

void painter_blit_bitmap(Painter *paint, Bitmap *src, Rectangle src_rect, Rectangle dst_rect)
{
    if (src_rect.width == dst_rect.width && src_rect.height == dst_rect.height)
    {
        painter_blit_bitmap_fast(paint, src, src_rect, dst_rect);
    }
    else
    {
        painter_blit_bitmap_scaled(paint, src, src_rect, dst_rect);
    }
}

void painter_clear(Painter *paint, Color color)
{
    painter_clear_rect(paint, bitmap_bound(paint->bitmap), color);
}

void painter_clear_rect(Painter *paint, Rectangle rect, Color color)
{
    Rectangle rect_absolue = rectangle_clip(paint->cliprect, rect);

    for (int xx = 0; xx < rect_absolue.width; xx++)
    {
        for (int yy = 0; yy < rect_absolue.height; yy++)
        {
            bitmap_set_pixel(
                paint->bitmap,
                (Point){rect_absolue.X + xx, rect_absolue.Y + yy},
                color);
        }
    }
}

void painter_fill_rect(Painter *paint, Rectangle rect, Color color)
{
    Rectangle rect_absolue = rectangle_clip(paint->cliprect, rect);

    for (int xx = 0; xx < rect_absolue.width; xx++)
    {
        for (int yy = 0; yy < rect_absolue.height; yy++)
        {
            bitmap_blend_pixel(
                paint->bitmap,
                (Point){rect_absolue.X + xx, rect_absolue.Y + yy},
                color);
        }
    }
}

// TODO: void painter_fill_circle(Painter* paint, ...);

// TODO: void painter_fill_triangle(Painter* paint, ...);

void painter_draw_line_x_aligned(Painter *paint, int x, int start, int end, Color color)
{
    for (int i = start; i < end; i++)
    {
        painter_plot_pixel(paint, (Point){x, i}, color);
    }
}

void painter_draw_line_y_aligned(Painter *paint, int y, int start, int end, Color color)
{
    for (int i = start; i < end; i++)
    {
        painter_plot_pixel(paint, (Point){i, y}, color);
    }
}

void painter_draw_line_not_aligned(Painter *paint, Point a, Point b, Color color)
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

void painter_draw_line(Painter *paint, Point a, Point b, Color color)
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

void painter_draw_rect(Painter *paint, Rectangle rect, Color color)
{
    painter_draw_line(paint, rect.position, point_sub(point_add(rect.position, point_x(rect.size)), (Point){1, 0}), color);
    painter_draw_line(paint, rect.position, point_sub(point_add(rect.position, point_y(rect.size)), (Point){0, 1}), color);
    painter_draw_line(paint, point_sub(point_add(rect.position, point_x(rect.size)), (Point){1, 0}), point_sub(point_add(rect.position, rect.size), (Point){1, 0}), color);
    painter_draw_line(paint, point_sub(point_add(rect.position, point_y(rect.size)), (Point){0, 1}), point_sub(point_add(rect.position, rect.size), (Point){0, 1}), color);
}

const int FONT_SIZE = 16;

void painter_blit_bitmap_colored(Painter *paint, Bitmap *src, Rectangle src_rect, Rectangle dst_rect, Color color)
{
    for (int x = 0; x < dst_rect.width; x++)
    {
        for (int y = 0; y < dst_rect.height; y++)
        {
            double xx = x / (double)dst_rect.width;
            double yy = y / (double)dst_rect.height;

            Color sample = bitmap_sample(src, src_rect, xx, yy);

            Color final = color;
            final.A = sample.R;

            painter_plot_pixel(paint, point_add(dst_rect.position, (Point){x, y}), final);
        }
    }
}

void painter_draw_glyph(Painter *paint, Font *font, Glyph *glyph, Point position, Color color)
{
    Rectangle dest;
    dest.position = point_sub(position, glyph->origin);
    dest.size = glyph->bound.size;

    painter_blit_bitmap_colored(paint, font->bitmap, glyph->bound, dest, color);
}
