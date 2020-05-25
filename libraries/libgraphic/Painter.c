
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

void painter_push_origin(Painter *painter, Vec2i origin)
{
    assert(painter->originestack_top < ORIGINSTACK_SIZE);

    painter->originestack_top++;
    painter->originestack[painter->originestack_top] = vec2i_add(painter->originestack[painter->originestack_top - 1], origin);
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

void painter_plot_pixel(Painter *painter, Vec2i position, Color color)
{
    Vec2i transformed = vec2i_add(position, painter->originestack[painter->originestack_top]);

    if (rectangle_containe_point(painter->clipstack[painter->clipstack_top], transformed))
    {
        bitmap_blend_pixel(painter->bitmap, transformed, color);
    }
}

__attribute__((flatten)) void painter_clear_pixel(Painter *painter, Vec2i position, Color color)
{
    Vec2i transformed = vec2i_add(position, painter->originestack[painter->originestack_top]);

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
    clipped_source.position = vec2i_add(source.position, vec2i_sub(clipped_destination.position, destination.position));

    if (rectangle_is_empty(destination))
    {
        return;
    }

    for (int x = 0; x < clipped_destination.width; x++)
    {
        for (int y = 0; y < clipped_destination.height; y++)
        {
            Color sample = bitmap_get_pixel(bitmap, vec2i(clipped_source.x + x, clipped_source.y + y));
            bitmap_blend_pixel_no_check(painter->bitmap, vec2i_add(clipped_destination.position, vec2i(x, y)), sample);
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
            painter_plot_pixel(painter, vec2i_add(destination.position, vec2i(x, y)), sample);
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
    clipped_source.position = vec2i_add(source.position, vec2i_sub(clipped_destination.position, destination.position));

    if (rectangle_is_empty(clipped_destination))
    {
        return;
    }

    for (int x = 0; x < clipped_destination.width; x++)
    {
        for (int y = 0; y < clipped_destination.height; y++)
        {
            Color sample = bitmap_get_pixel(bitmap, vec2i(clipped_source.x + x, clipped_source.y + y));
            bitmap_set_pixel_no_check(painter->bitmap, vec2i_add(clipped_destination.position, vec2i(x, y)), sample);
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
            painter_plot_pixel(painter, vec2i_add(destination.position, vec2i(x, y)), sample);
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
            bitmap_set_pixel_no_check(painter->bitmap, vec2i(rectangle.x + x, rectangle.y + y), color);
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
            bitmap_blend_pixel_no_check(painter->bitmap, vec2i(rectangle.x + x, rectangle.y + y),
                                        color);
        }
    }
}

__attribute__((flatten)) void painter_fill_triangle(Painter *painter, Vec2i p0, Vec2i p1, Vec2i p2, Color color)
{
    Vec2f a = vec2f(p0.x, p0.y);
    Vec2f b = vec2f(p1.x, p1.y);
    Vec2f c = vec2f(p2.x, p2.y);

    if (a.y > b.y)
        __swap(Vec2f, a, b);
    if (a.y > c.y)
        __swap(Vec2f, a, c);
    if (b.y > c.y)
        __swap(Vec2f, b, c);

    Vec2f s = a;
    Vec2f e = a;

    double dx1 = 0;
    double dx2 = 0;
    double dx3 = 0;

    if (b.y - a.y > 0)
    {
        dx1 = (b.x - a.x) / (double)(b.y - a.y);
    }

    if (c.y - a.y > 0)
    {
        dx2 = (c.x - a.x) / (double)(c.y - a.y);
    }

    if (c.y - b.y > 0)
    {
        dx3 = (c.x - b.x) / (double)(c.y - b.y);
    }

    if (dx1 > dx2)
    {
        for (; s.y <= b.y; s.y++, e.y++, s.x += dx2, e.x += dx1)
        {
            painter_draw_line(painter, vec2i(s.x - 1, s.y), vec2i(e.x + 1, s.y), color);
        }

        e = b;

        for (; s.y <= c.y; s.y++, e.y++, s.x += dx2, e.x += dx3)
        {
            painter_draw_line(painter, vec2i(s.x - 1, s.y), vec2i(e.x + 1, s.y), color);
        }
    }
    else
    {
        for (; s.y <= b.y; s.y++, e.y++, s.x += dx1, e.x += dx2)
        {
            painter_draw_line(painter, vec2i(s.x - 1, s.y), vec2i(e.x + 1, s.y), color);
        }

        s = b;

        for (; s.y <= c.y; s.y++, e.y++, s.x += dx3, e.x += dx2)
        {
            painter_draw_line(painter, vec2i(s.x - 1, s.y), vec2i(e.x + 1, s.y), color);
        }
    }
}

static double sample_fill_circle(Vec2i center, double radius, Vec2i position)
{
    double distance = vec2i_distance(center, position) - radius;
    return clamp(0.5 - distance, 0, 1);
}

void painter_fill_circle_helper(Painter *painter, Rectangle bound, Vec2i center, int radius, Color color)
{
    for (int x = 0; x < bound.width; x++)
    {
        for (int y = 0; y < bound.height; y++)
        {
            double distance = sample_fill_circle(center, radius - 0.5, vec2i(x, y));
            double alpha = (color.A / 255.0) * distance;

            painter_plot_pixel(painter, vec2i(bound.x + x, bound.y + y), ALPHA(color, alpha));
        }
    }
}

void painter_fill_rounded_rectangle(Painter *painter, Rectangle bound, int radius, Color color)
{
    radius = MIN(radius, bound.height / 2);
    radius = MIN(radius, bound.width / 2);

    Rectangle left_ear = rectangle_left(bound, radius);
    Rectangle right_ear = rectangle_right(bound, radius);

    painter_fill_circle_helper(painter, rectangle_top(left_ear, radius), vec2i(radius - 1, radius - 1), radius, color);
    painter_fill_circle_helper(painter, rectangle_bottom(left_ear, radius), vec2i(radius - 1, 0), radius, color);
    painter_fill_rectangle(painter, rectangle_cutoff_top_botton(left_ear, radius, radius), color);

    painter_fill_circle_helper(painter, rectangle_top(right_ear, radius), vec2i(0, radius - 1), radius, color);
    painter_fill_circle_helper(painter, rectangle_bottom(right_ear, radius), vec2i(0, 0), radius, color);
    painter_fill_rectangle(painter, rectangle_cutoff_top_botton(right_ear, radius, radius), color);

    painter_fill_rectangle(painter, rectangle_cutoff_left_right(bound, radius, radius), color);
}

void painter_draw_line_x_aligned(Painter *painter, int x, int start, int end, Color color)
{
    for (int i = start; i <= end; i++)
    {
        painter_plot_pixel(painter, vec2i(x, i), color);
    }
}

void painter_draw_line_y_aligned(Painter *painter, int y, int start, int end, Color color)
{
    for (int i = start; i <= end; i++)
    {
        painter_plot_pixel(painter, vec2i(i, y), color);
    }
}

void painter_draw_line_not_aligned(Painter *painter, Vec2i a, Vec2i b, Color color)
{
    int dx = abs(b.x - a.x), sx = a.x < b.x ? 1 : -1;
    int dy = abs(b.y - a.y), sy = a.y < b.y ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        painter_plot_pixel(painter, a, color);

        if (a.x == b.x && a.y == b.y)
            break;

        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            a.x += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            a.y += sy;
        }
    }
}

#define __ipart(X) ((int)(X))
#define __round(X) ((int)(((double)(X)) + 0.5))
#define __fpart(X) (((double)(X)) - (double)__ipart(X))
#define __rfpart(X) (1.0 - __fpart(X))
#define __plot(__x, __y, __brightness) painter_plot_pixel(painter, vec2i((__x), (__y)), ALPHA(color, (color.A / 255.0) * (__brightness)))

void painter_draw_line_antialias(Painter *painter, Vec2i a, Vec2i b, Color color)
{

    double x0 = a.x;
    double y0 = a.y;
    double x1 = b.x;
    double y1 = b.y;

    const bool steep = abs(y1 - a.y) > abs(x1 - x0);
    if (steep)
    {
        __swap(double, x0, y0);
        __swap(double, x1, y1);
    }
    if (x0 > x1)
    {
        __swap(double, x0, x1);
        __swap(double, y0, y1);
    }

    const float dx = x1 - x0;
    const float dy = y1 - y0;
    const float gradient = (dx == 0) ? 1 : dy / dx;

    int xpx11;
    float intery;
    {
        const float xend = __round(x0);
        const float yend = y0 + gradient * (xend - x0);
        const float xgap = __rfpart(x0 + 0.5);
        xpx11 = (int)xend;
        const int ypx11 = __ipart(yend);
        if (steep)
        {
            __plot(ypx11, xpx11, __rfpart(yend) * xgap);
            __plot(ypx11 + 1, xpx11, __fpart(yend) * xgap);
        }
        else
        {
            __plot(xpx11, ypx11, __rfpart(yend) * xgap);
            __plot(xpx11, ypx11 + 1, __fpart(yend) * xgap);
        }
        intery = yend + gradient;
    }

    int xpx12;
    {
        const float xend = __round(x1);
        const float yend = y1 + gradient * (xend - x1);
        const float xgap = __rfpart(x1 + 0.5);
        xpx12 = (int)xend;
        const int ypx12 = __ipart(yend);
        if (steep)
        {
            __plot(ypx12, xpx12, __rfpart(yend) * xgap);
            __plot(ypx12 + 1, xpx12, __fpart(yend) * xgap);
        }
        else
        {
            __plot(xpx12, ypx12, __rfpart(yend) * xgap);
            __plot(xpx12, ypx12 + 1, __fpart(yend) * xgap);
        }
    }

    if (steep)
    {
        for (int x = xpx11 + 1; x < xpx12; x++)
        {
            __plot(__ipart(intery), x, __rfpart(intery));
            __plot(__ipart(intery) + 1, x, __fpart(intery));
            intery += gradient;
        }
    }
    else
    {
        for (int x = xpx11 + 1; x < xpx12; x++)
        {
            __plot(x, __ipart(intery), __rfpart(intery));
            __plot(x, __ipart(intery) + 1, __fpart(intery));
            intery += gradient;
        }
    }
}

__attribute__((flatten)) void painter_draw_line(Painter *painter, Vec2i a, Vec2i b, Color color)
{
    if (a.x == b.x)
    {
        painter_draw_line_x_aligned(painter, a.x, MIN(a.y, b.y), MAX(a.y, b.y), color);
    }
    else if (a.y == b.y)
    {
        painter_draw_line_y_aligned(painter, a.y, MIN(a.x, b.x), MAX(a.x, b.x), color);
    }
    else
    {
        painter_draw_line_not_aligned(painter, a, b, color);
    }
}

__attribute__((flatten)) void painter_draw_rectangle(Painter *painter, Rectangle rect, Color color)
{
    Vec2i topleft = rect.position;
    Vec2i topright = vec2i_add(rect.position, vec2i_x(rect.size));
    topright = vec2i_sub(topright, vec2i(1, 0));

    Vec2i bottomleft = vec2i_add(rect.position, vec2i_y(rect.size));
    bottomleft = vec2i_sub(bottomleft, vec2i(0, 1));

    Vec2i bottomright = vec2i_add(rect.position, rect.size);
    bottomright = vec2i_sub(bottomright, vec2i(1, 1));

    painter_draw_line(painter, topleft, topright, color);
    painter_draw_line(painter, bottomleft, bottomright, color);

    painter_draw_line(painter, vec2i_add(topleft, vec2i(0, 1)), vec2i_sub(bottomleft, vec2i(0, 1)), color);
    painter_draw_line(painter, vec2i_add(topright, vec2i(0, 1)), vec2i_sub(bottomright, vec2i(0, 1)), color);
}

__attribute__((flatten)) void painter_draw_triangle(Painter *painter, Vec2i p0, Vec2i p1, Vec2i p2, Color color)
{
    painter_draw_line(painter, p0, p1, color);
    painter_draw_line(painter, p1, p2, color);
    painter_draw_line(painter, p2, p0, color);
}

static double sample_draw_circle(Vec2i center, double radius, double thickness, Vec2i position)
{
    double distance = abs(vec2i_distance(center, position) - (radius - thickness / 2));

    return clamp((0.5 - distance) + (thickness / 2), 0, 1);
}

void painter_draw_circle_helper(Painter *painter, Rectangle bound, Vec2i center, int radius, int thickness, Color color)
{
    for (int x = 0; x < bound.width; x++)
    {
        for (int y = 0; y < bound.height; y++)
        {
            double distance = sample_draw_circle(center, radius - 0.5, thickness, vec2i(x, y));
            double alpha = (color.A / 255.0) * distance;

            painter_plot_pixel(painter, vec2i(bound.x + x, bound.y + y), ALPHA(color, alpha));
        }
    }
}

void painter_draw_rounded_rectangle(Painter *painter, Rectangle bound, int radius, int thickness, Color color)
{
    radius = MIN(radius, bound.height / 2);
    radius = MIN(radius, bound.width / 2);

    Rectangle left_ear = rectangle_left(bound, radius);
    Rectangle right_ear = rectangle_right(bound, radius);

    painter_draw_circle_helper(painter, rectangle_top(left_ear, radius), vec2i(radius - 1, radius - 1), radius, thickness, color);
    painter_draw_circle_helper(painter, rectangle_bottom(left_ear, radius), vec2i(radius - 1, 0), radius, thickness, color);

    painter_draw_circle_helper(painter, rectangle_top(right_ear, radius), vec2i(0, radius - 1), radius, thickness, color);
    painter_draw_circle_helper(painter, rectangle_bottom(right_ear, radius), vec2i(0, 0), radius, thickness, color);

    painter_fill_rectangle(painter, rectangle_left(rectangle_cutoff_top_botton(left_ear, radius, radius), thickness), color);
    painter_fill_rectangle(painter, rectangle_right(rectangle_cutoff_top_botton(right_ear, radius, radius), thickness), color);
    painter_fill_rectangle(painter, rectangle_top(rectangle_cutoff_left_right(bound, radius, radius), thickness), color);
    painter_fill_rectangle(painter, rectangle_bottom(rectangle_cutoff_left_right(bound, radius, radius), thickness), color);
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

            painter_plot_pixel(painter, vec2i_add(destination.position, vec2i(x, y)), final);
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

            painter_plot_pixel(painter, vec2i_add(dst_rect.position, vec2i(x, y)), final);
        }
    }
}

void painter_draw_glyph(Painter *painter, Font *font, Glyph *glyph, Vec2i position, Color color)
{
    Rectangle dest;
    dest.position = vec2i_sub(position, glyph->origin);
    dest.size = glyph->bound.size;

    painter_blit_bitmap_colored(painter, font->bitmap, glyph->bound, dest, color);
}

__attribute__((flatten)) void painter_draw_string(Painter *painter, Font *font, const char *str, Vec2i position, Color color)
{
    for (size_t i = 0; str[i]; i++)
    {
        Glyph *glyph = font_glyph(font, str[i]);

        painter_draw_glyph(painter, font, glyph, position, color);
        position = vec2i_add(position, vec2i(glyph->advance, 0));
    }
}
