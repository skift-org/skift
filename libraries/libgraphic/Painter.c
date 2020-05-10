/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/Font.h>
#include <libgraphic/Painter.h>
#include <libmath/math.h>
#include <libsystem/assert.h>

Painter *painter_create(Bitmap *bitmap)
{
    Painter *painter = __create(Painter);

    painter->bitmap = bitmap;
    painter->clipstack_top = 0;
    painter->clipstack[0] = bitmap_bound(bitmap);

    return painter;
}

void painter_destroy(Painter *painter)
{
    free(painter);
}

void painter_push_clip(Painter *painter, Rectangle clip)
{
    assert(painter->clipstack_top < CLIPSTACK_SIZE);

    painter->clipstack_top++;
    painter->clipstack[painter->clipstack_top] = rectangle_clip(painter->clipstack[painter->clipstack_top - 1], clip);
}

void painter_pop_clip(Painter *painter)
{
    assert(painter->clipstack_top > 0);

    painter->clipstack_top--;
}

void painter_push_origin(Painter *painter, Point origin)
{
    assert(painter->originestack_top < ORIGINSTACK_SIZE);

    painter->originestack_top++;
    painter->originestack[painter->originestack_top] = point_add(painter->originestack[painter->originestack_top - 1], origin);
}

void painter_pop_origin(Painter *painter)
{
    assert(painter->originestack_top > 0);

    painter->originestack_top--;
}

Rectangle painter_apply_clip(Painter *painter, Rectangle rectangle)
{
    if (rectangle_colide(painter->clipstack[painter->clipstack_top], rectangle))
    {
        rectangle = rectangle_clip(painter->clipstack[painter->clipstack_top], rectangle);
        rectangle = rectangle_clip(bitmap_bound(painter->bitmap), rectangle);

        return rectangle;
    }
    else
    {
        return RECTANGLE_EMPTY;
    }
}

Rectangle painter_apply_transform(Painter *painter, Rectangle rectangle)
{
    return rectangle_offset(rectangle, painter->originestack[painter->originestack_top]);
}

void painter_plot_pixel(Painter *painter, Point position, Color color)
{
    Point transformed = point_add(position, painter->originestack[painter->originestack_top]);

    if (rectangle_containe_point(painter->clipstack[painter->clipstack_top], transformed))
    {
        bitmap_blend_pixel(painter->bitmap, transformed, color);
    }
}

__attribute__((flatten)) void painter_clear_pixel(Painter *painter, Point position, Color color)
{
    Point transformed = point_add(position, painter->originestack[painter->originestack_top]);

    if (rectangle_containe_point(painter->clipstack[painter->clipstack_top], transformed))
    {
        bitmap_set_pixel(painter->bitmap, transformed, color);
    }
}

void painter_blit_bitmap_fast(
    Painter *painter,
    Bitmap *bitmap,
    Rectangle source,
    Rectangle destination)
{
    Rectangle clipped_destination = painter_apply_transform(painter, destination);
    clipped_destination = painter_apply_clip(painter, clipped_destination);

    Rectangle clipped_source = RECTANGLE_SIZE(clipped_destination.width, clipped_destination.height);
    clipped_source.position = point_add(source.position, point_sub(clipped_destination.position, destination.position));

    if (rectangle_is_empty(destination))
    {
        return;
    }

    for (int x = 0; x < clipped_destination.width; x++)
    {
        for (int y = 0; y < clipped_destination.height; y++)
        {
            Color sample = bitmap_get_pixel(bitmap, (Point){clipped_source.X + x, clipped_source.Y + y});
            bitmap_blend_pixel_no_check(painter->bitmap, point_add(clipped_destination.position, (Point){x, y}), sample);
        }
    }
}

void painter_blit_bitmap_scaled(
    Painter *painter,
    Bitmap *bitmap,
    Rectangle source,
    Rectangle destination)
{
    for (int x = 0; x < destination.width; x++)
    {
        for (int y = 0; y < destination.height; y++)
        {
            float xx = x / (float)destination.width;
            float yy = y / (float)destination.height;

            Color sample = bitmap_sample(bitmap, source, xx, yy);
            painter_plot_pixel(painter, point_add(destination.position, (Point){x, y}), sample);
        }
    }
}

__attribute__((flatten)) void painter_blit_bitmap(
    Painter *painter,
    Bitmap *bitmap,
    Rectangle source,
    Rectangle destination)
{
    if (source.width == destination.width &&
        source.height == destination.height)
    {
        painter_blit_bitmap_fast(painter, bitmap, source, destination);
    }
    else
    {
        painter_blit_bitmap_scaled(painter, bitmap, source, destination);
    }
}

void painter_blit_bitmap_fast_no_alpha(
    Painter *painter,
    Bitmap *bitmap,
    Rectangle source,
    Rectangle destination)
{
    Rectangle clipped_destination = painter_apply_transform(painter, destination);
    clipped_destination = painter_apply_clip(painter, clipped_destination);

    Rectangle clipped_source = RECTANGLE_SIZE(clipped_destination.width, clipped_destination.height);
    clipped_source.position = point_add(source.position, point_sub(clipped_destination.position, destination.position));

    if (rectangle_is_empty(clipped_destination))
    {
        return;
    }

    for (int x = 0; x < clipped_destination.width; x++)
    {
        for (int y = 0; y < clipped_destination.height; y++)
        {
            Color sample = bitmap_get_pixel(bitmap, (Point){clipped_source.X + x, clipped_source.Y + y});
            bitmap_set_pixel_no_check(painter->bitmap, point_add(clipped_destination.position, (Point){x, y}), sample);
        }
    }
}

void painter_blit_bitmap_scaled_no_alpha(
    Painter *painter,
    Bitmap *bitmap,
    Rectangle source,
    Rectangle destination)
{
    for (int x = 0; x < destination.width; x++)
    {
        for (int y = 0; y < destination.height; y++)
        {
            float xx = x / (float)destination.width;
            float yy = y / (float)destination.height;

            Color sample = bitmap_sample(bitmap, source, xx, yy);
            painter_plot_pixel(painter, point_add(destination.position, (Point){x, y}), sample);
        }
    }
}

__attribute__((flatten)) void painter_blit_bitmap_no_alpha(
    Painter *painter,
    Bitmap *bitmap,
    Rectangle source,
    Rectangle destination)
{
    if (source.width == destination.width &&
        source.height == destination.height)
    {
        painter_blit_bitmap_fast_no_alpha(painter, bitmap, source, destination);
    }
    else
    {
        painter_blit_bitmap_scaled_no_alpha(painter, bitmap, source, destination);
    }
}

__attribute__((flatten)) void painter_clear(Painter *painter, Color color)
{
    painter_clear_rectangle(painter, bitmap_bound(painter->bitmap), color);
}

__attribute__((flatten)) void painter_clear_rectangle(Painter *painter, Rectangle rectangle, Color color)
{
    rectangle = painter_apply_transform(painter, rectangle);
    rectangle = painter_apply_clip(painter, rectangle);

    if (rectangle_is_empty(rectangle))
    {
        return;
    }

    for (int x = 0; x < rectangle.width; x++)
    {
        for (int y = 0; y < rectangle.height; y++)
        {
            bitmap_set_pixel_no_check(painter->bitmap, (Point){rectangle.X + x, rectangle.Y + y}, color);
        }
    }
}

__attribute__((flatten)) void painter_fill_rectangle(Painter *painter, Rectangle rectangle, Color color)
{
    rectangle = painter_apply_transform(painter, rectangle);
    rectangle = painter_apply_clip(painter, rectangle);

    if (rectangle_is_empty(rectangle))
    {
        return;
    }

    for (int x = 0; x < rectangle.width; x++)
    {
        for (int y = 0; y < rectangle.height; y++)
        {
            bitmap_blend_pixel_no_check(painter->bitmap, (Point){rectangle.X + x, rectangle.Y + y},
                                        color);
        }
    }
}

__attribute__((flatten)) void painter_fill_triangle(Painter *painter, Point p0, Point p1, Point p2, Color color)
{
    PointF a = {p0.X, p0.Y};
    PointF b = {p1.X, p1.Y};
    PointF c = {p2.X, p2.Y};

    if (a.Y > b.Y)
        __swap(PointF, a, b);
    if (a.Y > c.Y)
        __swap(PointF, a, c);
    if (b.Y > c.Y)
        __swap(PointF, b, c);

    PointF s = a;
    PointF e = a;

    double dx1 = 0;
    double dx2 = 0;
    double dx3 = 0;

    if (b.Y - a.Y > 0)
    {
        dx1 = (b.X - a.X) / (double)(b.Y - a.Y);
    }

    if (c.Y - a.Y > 0)
    {
        dx2 = (c.X - a.X) / (double)(c.Y - a.Y);
    }

    if (c.Y - b.Y > 0)
    {
        dx3 = (c.X - b.X) / (double)(c.Y - b.Y);
    }

    if (dx1 > dx2)
    {
        for (; s.Y <= b.Y; s.Y++, e.Y++, s.X += dx2, e.X += dx1)
        {
            painter_draw_line(painter, (Point){s.X - 1, s.Y}, (Point){e.X + 1, s.Y}, color);
        }

        e = b;

        for (; s.Y <= c.Y; s.Y++, e.Y++, s.X += dx2, e.X += dx3)
        {
            painter_draw_line(painter, (Point){s.X - 1, s.Y}, (Point){e.X + 1, s.Y}, color);
        }
    }
    else
    {
        for (; s.Y <= b.Y; s.Y++, e.Y++, s.X += dx1, e.X += dx2)
        {
            painter_draw_line(painter, (Point){s.X - 1, s.Y}, (Point){e.X + 1, s.Y}, color);
        }

        s = b;

        for (; s.Y <= c.Y; s.Y++, e.Y++, s.X += dx3, e.X += dx2)
        {
            painter_draw_line(painter, (Point){s.X - 1, s.Y}, (Point){e.X + 1, s.Y}, color);
        }
    }
}

void painter_draw_line_x_aligned(Painter *painter, int x, int start, int end, Color color)
{
    for (int i = start; i <= end; i++)
    {
        painter_plot_pixel(painter, (Point){x, i}, color);
    }
}

void painter_draw_line_y_aligned(Painter *painter, int y, int start, int end, Color color)
{
    for (int i = start; i <= end; i++)
    {
        painter_plot_pixel(painter, (Point){i, y}, color);
    }
}

void painter_draw_line_not_aligned(Painter *painter, Point a, Point b, Color color)
{
    int dx = abs(b.X - a.X), sx = a.X < b.X ? 1 : -1;
    int dy = abs(b.Y - a.Y), sy = a.Y < b.Y ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        painter_plot_pixel(painter, a, color);

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

__attribute__((flatten)) void painter_draw_line(Painter *painter, Point a, Point b, Color color)
{
    if (a.X == b.X)
    {
        painter_draw_line_x_aligned(painter, a.X, MIN(a.Y, b.Y), MAX(a.Y, b.Y), color);
    }
    else if (a.Y == b.Y)
    {
        painter_draw_line_y_aligned(painter, a.Y, MIN(a.X, b.X), MAX(a.X, b.X), color);
    }
    else
    {
        painter_draw_line_not_aligned(painter, a, b, color);
    }
}

__attribute__((flatten)) void painter_draw_rectangle(Painter *painter, Rectangle rect, Color color)
{
    Point topleft = rect.position;
    Point topright = point_add(rect.position, point_x(rect.size));
    topright = point_sub(topright, (Point){1, 0});

    Point bottomleft = point_add(rect.position, point_y(rect.size));
    bottomleft = point_sub(bottomleft, (Point){0, 1});

    Point bottomright = point_add(rect.position, rect.size);
    bottomright = point_sub(bottomright, (Point){1, 1});

    painter_draw_line(painter, topleft, topright, color);
    painter_draw_line(painter, bottomleft, bottomright, color);

    painter_draw_line(painter, point_add(topleft, (Point){0, 1}), point_sub(bottomleft, (Point){0, 1}), color);
    painter_draw_line(painter, point_add(topright, (Point){0, 1}), point_sub(bottomright, (Point){0, 1}), color);
}

__attribute__((flatten)) void painter_draw_triangle(Painter *painter, Point p0, Point p1, Point p2, Color color)
{
    painter_draw_line(painter, p0, p1, color);
    painter_draw_line(painter, p1, p2, color);
    painter_draw_line(painter, p2, p0, color);
}

__attribute__((flatten)) void painter_blit_icon(Painter *painter, Bitmap *icon, Rectangle destination, Color color)
{
    for (int x = 0; x < destination.width; x++)
    {
        for (int y = 0; y < destination.height; y++)
        {
            double xx = x / (double)destination.width;
            double yy = y / (double)destination.height;

            Color sample = bitmap_sample(icon, bitmap_bound(icon), xx, yy);

            Color final = color;
            final.A = sample.A;

            painter_plot_pixel(painter, point_add(destination.position, (Point){x, y}), final);
        }
    }
}

__attribute__((flatten)) void painter_blit_bitmap_colored(Painter *painter, Bitmap *src, Rectangle src_rect, Rectangle dst_rect, Color color)
{
    for (int x = 0; x < dst_rect.width; x++)
    {
        for (int y = 0; y < dst_rect.height; y++)
        {
            double xx = x / (double)dst_rect.width;
            double yy = y / (double)dst_rect.height;

            Color sample = bitmap_sample(src, src_rect, xx, yy);

            Color final = color;
            final.A = (sample.R * color.A) / 255;

            painter_plot_pixel(painter, point_add(dst_rect.position, (Point){x, y}), final);
        }
    }
}

void painter_draw_glyph(Painter *painter, Font *font, Glyph *glyph, Point position, Color color)
{
    Rectangle dest;
    dest.position = point_sub(position, glyph->origin);
    dest.size = glyph->bound.size;

    painter_blit_bitmap_colored(painter, font->bitmap, glyph->bound, dest, color);
}

__attribute__((flatten)) void painter_draw_string(Painter *painter, Font *font, const char *str, Point position, Color color)
{
    for (size_t i = 0; str[i]; i++)
    {
        Glyph *glyph = font_glyph(font, str[i]);

        painter_draw_glyph(painter, font, glyph, position, color);
        position = point_add(position, (Point){glyph->advance, 0});
    }
}
