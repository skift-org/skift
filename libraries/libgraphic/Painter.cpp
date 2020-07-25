
#include <libgraphic/Font.h>
#include <libgraphic/Painter.h>
#include <libgraphic/StackBlur.h>
#include <libsystem/Assert.h>
#include <libsystem/math/Math.h>

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
    painter->clipstack[painter->clipstack_top] = painter->clipstack[painter->clipstack_top - 1].clipped_with(clip);
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
    painter->originestack[painter->originestack_top] = painter->originestack[painter->originestack_top - 1] + origin;
}

void painter_pop_origin(Painter *painter)
{
    assert(painter->originestack_top > 0);

    painter->originestack_top--;
}

Rectangle painter_apply_clip(Painter *painter, Rectangle rectangle)
{
    if (painter->clipstack[painter->clipstack_top].colide_with(rectangle))
    {
        rectangle = rectangle.clipped_with(painter->clipstack[painter->clipstack_top]);
        rectangle = rectangle.clipped_with(bitmap_bound(painter->bitmap));

        return rectangle;
    }
    else
    {
        return Rectangle::empty();
    }
}

Rectangle painter_apply_transform(Painter *painter, Rectangle rectangle)
{
    return rectangle.offset(painter->originestack[painter->originestack_top]);
}

void painter_plot_pixel(Painter *painter, Vec2i position, Color color)
{
    Vec2i transformed = position + painter->originestack[painter->originestack_top];

    if (painter->clipstack[painter->clipstack_top].containe(transformed))
    {
        bitmap_blend_pixel(painter->bitmap, transformed, color);
    }
}

__flatten void painter_clear_pixel(Painter *painter, Vec2i position, Color color)
{
    Vec2i transformed = position + painter->originestack[painter->originestack_top];

    if (painter->clipstack[painter->clipstack_top].containe(transformed))
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

    Rectangle clipped_source = Rectangle(clipped_destination.width(), clipped_destination.height());
    clipped_source = clipped_source.moved(source.position() + clipped_destination.position() - destination.position());

    if (clipped_destination.is_empty())
        return;

    for (int x = 0; x < clipped_destination.width(); x++)
    {
        for (int y = 0; y < clipped_destination.height(); y++)
        {
            Vec2i position(x, y);

            Color sample = bitmap_get_pixel(
                bitmap,
                clipped_source.position() + position);

            bitmap_blend_pixel(
                painter->bitmap,
                clipped_destination.position() + position, sample);
        }
    }
}

void painter_blit_bitmap_scaled(
    Painter *painter,
    Bitmap *bitmap,
    Rectangle source,
    Rectangle destination)
{
    if (destination.is_empty())
        return;

    for (int x = 0; x < destination.width(); x++)
    {
        for (int y = 0; y < destination.height(); y++)
        {
            float xx = x / (float)destination.width();
            float yy = y / (float)destination.height();

            Color sample = bitmap_sample(bitmap, source, xx, yy);
            painter_plot_pixel(painter, destination.position() + Vec2i(x, y), sample);
        }
    }
}

__flatten void painter_blit_bitmap(
    Painter *painter,
    Bitmap *bitmap,
    Rectangle source,
    Rectangle destination)
{
    if (destination.is_empty())
        return;

    if (source.width() == destination.width() &&
        source.height() == destination.height())
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

    Rectangle clipped_source = Rectangle(clipped_destination.width(), clipped_destination.height());
    clipped_source = clipped_source.moved(source.position() + clipped_destination.position() - destination.position());

    if (clipped_destination.is_empty())
        return;

    for (int x = 0; x < clipped_destination.width(); x++)
    {
        for (int y = 0; y < clipped_destination.height(); y++)
        {
            Vec2i position(x, y);

            Color sample = bitmap_get_pixel(bitmap, clipped_source.position() + position);

            sample.A = 255;

            bitmap_set_pixel_no_check(painter->bitmap, clipped_destination.position() + position, sample);
        }
    }
}

void painter_blit_bitmap_scaled_no_alpha(
    Painter *painter,
    Bitmap *bitmap,
    Rectangle source,
    Rectangle destination)
{
    for (int x = 0; x < destination.width(); x++)
    {
        for (int y = 0; y < destination.height(); y++)
        {
            float xx = x / (float)destination.width();
            float yy = y / (float)destination.height();

            Color sample = bitmap_sample(bitmap, source, xx, yy);
            painter_plot_pixel(painter, destination.position() + Vec2i(x, y), sample);
        }
    }
}

__flatten void painter_blit_bitmap_no_alpha(
    Painter *painter,
    Bitmap *bitmap,
    Rectangle source,
    Rectangle destination)
{
    if (source.width() == destination.width() &&
        source.height() == destination.height())
    {
        painter_blit_bitmap_fast_no_alpha(painter, bitmap, source, destination);
    }
    else
    {
        painter_blit_bitmap_scaled_no_alpha(painter, bitmap, source, destination);
    }
}

__flatten void painter_clear(Painter *painter, Color color)
{
    painter_clear_rectangle(painter, bitmap_bound(painter->bitmap), color);
}

__flatten void painter_clear_rectangle(Painter *painter, Rectangle rectangle, Color color)
{
    rectangle = painter_apply_transform(painter, rectangle);
    rectangle = painter_apply_clip(painter, rectangle);

    if (rectangle.is_empty())
    {
        return;
    }

    for (int x = 0; x < rectangle.width(); x++)
    {
        for (int y = 0; y < rectangle.height(); y++)
        {
            bitmap_set_pixel_no_check(painter->bitmap, Vec2i(rectangle.x() + x, rectangle.y() + y), color);
        }
    }
}

__flatten void painter_fill_rectangle(Painter *painter, Rectangle rectangle, Color color)
{
    rectangle = painter_apply_transform(painter, rectangle);
    rectangle = painter_apply_clip(painter, rectangle);

    if (rectangle.is_empty())
    {
        return;
    }

    for (int x = 0; x < rectangle.width(); x++)
    {
        for (int y = 0; y < rectangle.height(); y++)
        {
            bitmap_blend_pixel_no_check(painter->bitmap, Vec2i(rectangle.x() + x, rectangle.y() + y),
                                        color);
        }
    }
}

__flatten void painter_fill_insets(Painter *painter, Rectangle rectangle, Insets insets, Color color)
{
    Rectangle left_ear = rectangle.take_left(insets.left());
    Rectangle right_ear = rectangle.take_right(insets.right());

    Rectangle top = rectangle.cutoff_left_and_right(insets.left(), insets.right()).take_top(insets.top());
    Rectangle bottom = rectangle.cutoff_left_and_right(insets.left(), insets.right()).take_bottom(insets.bottom());

    painter_fill_rectangle(painter, left_ear, color);
    painter_fill_rectangle(painter, right_ear, color);
    painter_fill_rectangle(painter, top, color);
    painter_fill_rectangle(painter, bottom, color);
}

__flatten void painter_fill_triangle(Painter *painter, Vec2i p0, Vec2i p1, Vec2i p2, Color color)
{
    Vec2f a(p0);
    Vec2f b(p1);
    Vec2f c(p2);

    if (a.y() > b.y())
        swap(a, b);
    if (a.y() > c.y())
        swap(a, c);
    if (b.y() > c.y())
        swap(b, c);

    Vec2f s = a;
    Vec2f e = a;

    double dx1 = 0;
    double dx2 = 0;
    double dx3 = 0;

    if (b.y() - a.y() > 0)
    {
        dx1 = (b.x() - a.x()) / (double)(b.y() - a.y());
    }

    if (c.y() - a.y() > 0)
    {
        dx2 = (c.x() - a.x()) / (double)(c.y() - a.y());
    }

    if (c.y() - b.y() > 0)
    {
        dx3 = (c.x() - b.x()) / (double)(c.y() - b.y());
    }

    if (dx1 > dx2)
    {
        do
        {
            painter_draw_line(painter, Vec2i(s.x() - 1, s.y()), Vec2i(e.x() + 1, s.y()), color);

            s += Vec2f(dx2, 1);
            e += Vec2f(dx1, 1);
        } while (s.y() <= b.y());

        e = b;

        do
        {
            painter_draw_line(painter, Vec2i(s.x() - 1, s.y()), Vec2i(e.x() + 1, s.y()), color);

            s += Vec2f(dx2, 1);
            e += Vec2f(dx3, 1);
        } while (s.y() <= c.y());
    }
    else
    {
        do
        {
            painter_draw_line(painter, Vec2i(s.x() - 1, s.y()), Vec2i(e.x() + 1, s.y()), color);

            s += Vec2f(dx1, 1);
            e += Vec2f(dx2, 1);
        } while (s.y() <= b.y());

        s = b;

        do
        {
            painter_draw_line(painter, Vec2i(s.x() - 1, s.y()), Vec2i(e.x() + 1, s.y()), color);

            s += Vec2f(dx3, 1);
            e += Vec2f(dx2, 1);
        } while (s.y() <= c.y());
    }
}

static double sample_fill_circle(Vec2i center, double radius, Vec2i position)
{
    double distance = center.distance_to(position) - radius;
    return clamp(0.5 - distance, 0, 1);
}

void painter_fill_circle_helper(Painter *painter, Rectangle bound, Vec2i center, int radius, Color color)
{
    for (int x = 0; x < bound.width(); x++)
    {
        for (int y = 0; y < bound.height(); y++)
        {
            double distance = sample_fill_circle(center, radius - 0.5, Vec2i(x, y));
            double alpha = (color.A / 255.0) * distance;

            painter_plot_pixel(painter, Vec2i(bound.x() + x, bound.y() + y), ALPHA(color, alpha));
        }
    }
}

void painter_fill_rounded_rectangle(Painter *painter, Rectangle bound, int radius, Color color)
{
    radius = MIN(radius, bound.height() / 2);
    radius = MIN(radius, bound.width() / 2);

    Rectangle left_ear = bound.take_left(radius);
    Rectangle right_ear = bound.take_right(radius);

    painter_fill_circle_helper(painter, left_ear.take_top(radius), Vec2i(radius - 1, radius - 1), radius, color);
    painter_fill_circle_helper(painter, left_ear.take_bottom(radius), Vec2i(radius - 1, 0), radius, color);
    painter_fill_rectangle(painter, left_ear.cutoff_top_and_botton(radius, radius), color);

    painter_fill_circle_helper(painter, right_ear.take_top(radius), Vec2i(0, radius - 1), radius, color);
    painter_fill_circle_helper(painter, right_ear.take_bottom(radius), Vec2i::zero(), radius, color);
    painter_fill_rectangle(painter, right_ear.cutoff_top_and_botton(radius, radius), color);

    painter_fill_rectangle(painter, bound.cutoff_left_and_right(radius, radius), color);
}

void painter_fill_checkboard(Painter *painter, Rectangle bound, int cell_size, Color fg_color, Color bg_color)
{
    for (int x = 0; x < bound.width(); x++)
    {
        for (int y = 0; y < bound.height(); y++)
        {
            Vec2i position = bound.position() + Vec2i(x, y);

            if (((y / cell_size) * bound.width() + (x / cell_size) + (y / cell_size) % 2) % 2 == 0)
            {
                painter_plot_pixel(painter, position, fg_color);
            }
            else
            {
                painter_plot_pixel(painter, position, bg_color);
            }
        }
    }
}

void painter_draw_line_x_aligned(Painter *painter, int x, int start, int end, Color color)
{
    for (int i = start; i <= end; i++)
    {
        painter_plot_pixel(painter, Vec2i(x, i), color);
    }
}

void painter_draw_line_y_aligned(Painter *painter, int y, int start, int end, Color color)
{
    for (int i = start; i <= end; i++)
    {
        painter_plot_pixel(painter, Vec2i(i, y), color);
    }
}

void painter_draw_line_not_aligned(Painter *painter, Vec2i a, Vec2i b, Color color)
{
    int dx = abs(b.x() - a.x());
    int dy = abs(b.y() - a.y());

    int sx = a.x() < b.x() ? 1 : -1;
    int sy = a.y() < b.y() ? 1 : -1;

    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        painter_plot_pixel(painter, a, color);

        if (a.x() == b.x() && a.y() == b.y())
            break;

        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            a += Vec2(sx, 0);
        }
        if (e2 < dy)
        {
            err += dx;
            a += Vec2(0, sy);
        }
    }
}

#define __ipart(X) ((int)(X))
#define __round(X) ((int)(((double)(X)) + 0.5))
#define __fpart(X) (((double)(X)) - (double)__ipart(X))
#define __rfpart(X) (1.0 - __fpart(X))
#define __plot(__x, __y, __brightness) painter_plot_pixel(painter, Vec2i((__x), (__y)), ALPHA(color, (color.A / 255.0) * (__brightness)))

void painter_draw_line_antialias(Painter *painter, Vec2i a, Vec2i b, Color color)
{
    double x0 = a.x();
    double y0 = a.y();
    double x1 = b.x();
    double y1 = b.y();

    const bool steep = abs(y1 - a.y()) > abs(x1 - x0);
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

__flatten void painter_draw_line(Painter *painter, Vec2i a, Vec2i b, Color color)
{
    if (a.x() == b.x())
    {
        painter_draw_line_x_aligned(painter, a.x(), MIN(a.y(), b.y()), MAX(a.y(), b.y()), color);
    }
    else if (a.y() == b.y())
    {
        painter_draw_line_y_aligned(painter, a.y(), MIN(a.x(), b.x()), MAX(a.x(), b.x()), color);
    }
    else
    {
        painter_draw_line_not_aligned(painter, a, b, color);
    }
}

__flatten void painter_draw_rectangle(Painter *painter, Rectangle rect, Color color)
{
    Vec2i topleft = rect.position();
    Vec2i topright = rect.position() + rect.size().extract_x() - Vec2i::oneX();
    Vec2i bottomleft = rect.position() + rect.size().extract_y() - Vec2i::oneY();
    Vec2i bottomright = rect.position() + rect.size() - Vec2i::one();

    painter_draw_line(painter, topleft, topright, color);
    painter_draw_line(painter, bottomleft, bottomright, color);

    painter_draw_line(painter, topleft + Vec2i::oneY(), bottomleft - Vec2i::oneY(), color);
    painter_draw_line(painter, topright + Vec2i::oneY(), bottomright - Vec2i::oneY(), color);
}

__flatten void painter_draw_triangle(Painter *painter, Vec2i p0, Vec2i p1, Vec2i p2, Color color)
{
    painter_draw_line(painter, p0, p1, color);
    painter_draw_line(painter, p1, p2, color);
    painter_draw_line(painter, p2, p0, color);
}

static double sample_draw_circle(Vec2i center, double radius, double thickness, Vec2i position)
{
    double distance = abs(center.distance_to(position) - (radius - thickness / 2));

    return clamp((0.5 - distance) + (thickness / 2), 0, 1);
}

void painter_draw_circle_helper(Painter *painter, Rectangle bound, Vec2i center, int radius, int thickness, Color color)
{
    for (int x = 0; x < bound.width(); x++)
    {
        for (int y = 0; y < bound.height(); y++)
        {
            Vec2i position = Vec2i(x, y);

            double distance = sample_draw_circle(center, radius - 0.5, thickness, position);
            double alpha = (color.A / 255.0) * distance;

            painter_plot_pixel(painter, bound.position() + position, ALPHA(color, alpha));
        }
    }
}

void painter_draw_rounded_rectangle(Painter *painter, Rectangle bound, int radius, int thickness, Color color)
{
    radius = MIN(radius, bound.height() / 2);
    radius = MIN(radius, bound.width() / 2);

    Rectangle left_ear = bound.take_left(radius);
    Rectangle right_ear = bound.take_right(radius);

    painter_draw_circle_helper(painter, left_ear.take_top(radius), Vec2i(radius - 1, radius - 1), radius, thickness, color);
    painter_draw_circle_helper(painter, left_ear.take_bottom(radius), Vec2i(radius - 1, 0), radius, thickness, color);

    painter_draw_circle_helper(painter, right_ear.take_top(radius), Vec2i(0, radius - 1), radius, thickness, color);
    painter_draw_circle_helper(painter, right_ear.take_bottom(radius), Vec2i::zero(), radius, thickness, color);

    painter_fill_rectangle(painter, left_ear.cutoff_top_and_botton(radius, radius).take_left(thickness), color);
    painter_fill_rectangle(painter, right_ear.cutoff_top_and_botton(radius, radius).take_right(thickness), color);

    painter_fill_rectangle(painter, bound.cutoff_left_and_right(radius, radius).take_top(thickness), color);
    painter_fill_rectangle(painter, bound.cutoff_left_and_right(radius, radius).take_bottom(thickness), color);
}

__flatten void painter_blit_icon(Painter *painter, Icon *icon, IconSize size, Rectangle destination, Color color)
{
    Bitmap *bitmap = icon_get_bitmap(icon, size);

    for (int x = 0; x < destination.width(); x++)
    {
        for (int y = 0; y < destination.height(); y++)
        {
            double xx = x / (double)destination.width();
            double yy = y / (double)destination.height();

            Color sample = bitmap_sample(bitmap, bitmap_bound(bitmap), xx, yy);

            Color final = color;
            final.A = sample.A;

            painter_plot_pixel(painter, destination.position() + Vec2i(x, y), final);
        }
    }
}

__flatten void painter_blit_bitmap_colored(Painter *painter, Bitmap *src, Rectangle src_rect, Rectangle dst_rect, Color color)
{
    for (int x = 0; x < dst_rect.width(); x++)
    {
        for (int y = 0; y < dst_rect.height(); y++)
        {
            double xx = x / (double)dst_rect.width();
            double yy = y / (double)dst_rect.height();

            Color sample = bitmap_sample(src, src_rect, xx, yy);

            Color final = color;
            final.A = (sample.R * color.A) / 255;

            painter_plot_pixel(painter, dst_rect.position() + Vec2i(x, y), final);
        }
    }
}

void painter_blur_rectangle(Painter *painter, Rectangle rectangle, int radius)
{
    rectangle = painter_apply_transform(painter, rectangle);
    rectangle = painter_apply_clip(painter, rectangle);

    stackblurJob((unsigned char *)painter->bitmap->pixels,
                 painter->bitmap->width,
                 painter->bitmap->height,
                 radius,
                 rectangle.x(), rectangle.x() + rectangle.width(),
                 rectangle.y(), rectangle.y() + rectangle.height());
}

void painter_draw_glyph(Painter *painter, Font *font, Glyph *glyph, Vec2i position, Color color)
{
    Rectangle dest(position - glyph->origin, glyph->bound.size());
    painter_blit_bitmap_colored(painter, font->bitmap, glyph->bound, dest, color);
}

__flatten void painter_draw_string(Painter *painter, Font *font, const char *str, Vec2i position, Color color)
{
    Codepoint codepoint = 0;
    size_t size = utf8_to_codepoint((const uint8_t *)str, &codepoint);
    str += size;
    while (size && codepoint != 0)
    {
        Glyph *glyph = font_glyph(font, codepoint);

        painter_draw_glyph(painter, font, glyph, position, color);
        position = position + Vec2i(glyph->advance, 0);

        size_t size = utf8_to_codepoint((const uint8_t *)str, &codepoint);
        str += size;
    }
}

void painter_draw_string_within(Painter *painter, Font *font, const char *str, Rectangle container, Position position, Color color)
{
    Rectangle bound = Rectangle(font_measure_string(font, str), 16);

    bound = bound.place_within(container, position);

    painter_draw_string(
        painter,
        font,
        str,
        Vec2i(bound.x(), bound.y() + bound.height() / 2 + 4),
        color);
}

void painter_draw_truetype_glyph(Painter *painter, TrueTypeFont *font, TrueTypeGlyph *glyph, Vec2i position, Color color)
{
    Rectangle dest(position + glyph->offset, glyph->bound.size());
    TrueTypeAtlas *atlas = truetypefont_get_atlas(font);

    for (int x = 0; x < dest.width(); x++)
    {
        for (int y = 0; y < dest.height(); y++)
        {
            int alpha = (atlas->buffer[(y + glyph->bound.y()) * atlas->width + (x + glyph->bound.x())] * color.A) / 255;

            painter_plot_pixel(painter, dest.position() + Vec2i(x, y), COLOR_RGBA(color.R, color.G, color.B, alpha));
        }
    }

    //painter_draw_rectangle(painter, dest, COLOR_RED);
}

#include <libsystem/Logger.h>

__flatten void painter_draw_truetype_string(Painter *painter, TrueTypeFont *font, const char *string, Vec2i position, Color color)
{
    // Rectangle bound = truetypefont_mesure_string(font, string);
    // TrueTypeFontMetrics metrics = truetypefont_get_metrics(font);
    //
    // bound = rectangle_offset(bound, position);
    // bound = rectangle_offset(bound, Vec2i(0, -metrics.ascent));
    //
    // painter_draw_rectangle(painter, bound, COLOR_GREEN);

    Codepoint codepoint = 0;
    Codepoint previouse = 0;
    size_t size = utf8_to_codepoint((const uint8_t *)string, &codepoint);
    string += size;
    while (size && codepoint != 0)
    {
        TrueTypeGlyph *glyph = truetypefont_get_glyph_for_codepoint(font, codepoint);

        if (glyph)
        {
            if (previouse != 0)
            {
                int kerning = truetypefont_get_kerning_for_codepoints(font, previouse, codepoint);
                position = position + Vec2i(kerning, 0);
            }

            painter_draw_truetype_glyph(painter, font, glyph, position, color);

            position = position + Vec2i(glyph->advance, 0);
        }

        previouse = codepoint;

        size_t size = utf8_to_codepoint((const uint8_t *)string, &codepoint);
        string += size;
    }
}

void painter_draw_truetype_string_within(Painter *painter, TrueTypeFont *font, const char *str, Rectangle container, Position position, Color color)
{
    Rectangle bound =
        truetypefont_mesure_string(font, str)
            .place_within(container, position);

    painter_draw_truetype_string(
        painter,
        font,
        str,
        Vec2i(bound.x(), bound.y() + bound.height() / 2 + 4),
        color);
}
