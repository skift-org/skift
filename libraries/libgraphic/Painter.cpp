#include <libgraphic/Font.h>
#include <libgraphic/Painter.h>
#include <libgraphic/StackBlur.h>
#include <libsystem/Assert.h>
#include <libsystem/math/Math.h>

Painter::Painter(RefPtr<Bitmap> bitmap)
{
    _bitmap = bitmap;
    _state_stack_top = 0;
    _state_stack[0] = {
        Vec2i::zero(),
        bitmap->bound(),
    };
}

void Painter::push()
{
    assert(_state_stack_top < STATESTACK_SIZE);

    _state_stack_top++;
    _state_stack[_state_stack_top] = _state_stack[_state_stack_top - 1];
}

void Painter::pop()
{
    assert(_state_stack_top > 0);
    _state_stack_top--;
}

void Painter::clip(Rectangle rectangle)
{
    Rectangle transformed_rectangle = rectangle.offset(origine());
    Rectangle clipped_rectangle = transformed_rectangle.clipped_with(clip());

    _state_stack[_state_stack_top].clip = clipped_rectangle;
}

void Painter::transform(Vec2i offset)
{
    _state_stack[_state_stack_top].origine += offset;
}

Rectangle Painter::apply_clip(Rectangle rectangle)
{
    if (rectangle.colide_with(clip()))
    {
        rectangle = rectangle.clipped_with(clip());
        rectangle = rectangle.clipped_with(_bitmap->bound());

        return rectangle;
    }
    else
    {
        return Rectangle::empty();
    }
}

Rectangle Painter::apply_transform(Rectangle rectangle)
{

    return rectangle.offset(_state_stack[_state_stack_top].origine);
}

void Painter::plot_pixel(Vec2i position, Color color)
{
    Vec2i transformed = position + _state_stack[_state_stack_top].origine;

    if (clip().contains(transformed))
    {
        _bitmap->blend_pixel(transformed, color);
    }
}

void Painter::blit_bitmap_fast(Bitmap &bitmap, Rectangle source, Rectangle destination)
{
    Rectangle clipped_destination = apply_transform(destination);
    clipped_destination = apply_clip(clipped_destination);

    Rectangle clipped_source = Rectangle(clipped_destination.width(), clipped_destination.height());
    clipped_source = clipped_source.moved(source.position() + clipped_destination.position() - destination.position());

    if (clipped_destination.is_empty())
        return;

    for (int x = 0; x < clipped_destination.width(); x++)
    {
        for (int y = 0; y < clipped_destination.height(); y++)
        {
            Vec2i position(x, y);

            Color sample = bitmap.get_pixel(clipped_source.position() + position);

            _bitmap->blend_pixel(clipped_destination.position() + position, sample);
        }
    }
}

void Painter::blit_bitmap_scaled(Bitmap &bitmap, Rectangle source, Rectangle destination)
{
    if (destination.is_empty())
        return;

    for (int x = 0; x < destination.width(); x++)
    {
        for (int y = 0; y < destination.height(); y++)
        {
            float xx = x / (float)destination.width();
            float yy = y / (float)destination.height();

            Color sample = bitmap.sample(source, Vec2f(xx, yy));
            plot_pixel(destination.position() + Vec2i(x, y), sample);
        }
    }
}

__flatten void Painter::blit_bitmap(Bitmap &bitmap, Rectangle source, Rectangle destination)
{
    if (destination.is_empty())
        return;

    if (source.width() == destination.width() &&
        source.height() == destination.height())
    {
        blit_bitmap_fast(bitmap, source, destination);
    }
    else
    {
        blit_bitmap_scaled(bitmap, source, destination);
    }
}

void Painter::blit_bitmap_fast_no_alpha(Bitmap &bitmap, Rectangle source, Rectangle destination)
{
    Rectangle clipped_destination = apply_transform(destination);
    clipped_destination = apply_clip(clipped_destination);

    Rectangle clipped_source = Rectangle(clipped_destination.width(), clipped_destination.height());
    clipped_source = clipped_source.moved(source.position() + clipped_destination.position() - destination.position());

    if (clipped_destination.is_empty())
        return;

    for (int x = 0; x < clipped_destination.width(); x++)
    {
        for (int y = 0; y < clipped_destination.height(); y++)
        {
            Vec2i position(x, y);

            Color sample = bitmap.get_pixel(clipped_source.position() + position);
            _bitmap->set_pixel_no_check(clipped_destination.position() + position, sample.with_alpha(1));
        }
    }
}

void Painter::blit_bitmap_scaled_no_alpha(Bitmap &bitmap, Rectangle source, Rectangle destination)
{
    for (int x = 0; x < destination.width(); x++)
    {
        for (int y = 0; y < destination.height(); y++)
        {
            float xx = x / (float)destination.width();
            float yy = y / (float)destination.height();

            Color sample = bitmap.sample(source, Vec2f(xx, yy));
            plot_pixel(destination.position() + Vec2i(x, y), sample);
        }
    }
}

__flatten void Painter::blit_bitmap_no_alpha(Bitmap &bitmap, Rectangle source, Rectangle destination)
{
    if (source.width() == destination.width() &&
        source.height() == destination.height())
    {
        blit_bitmap_fast_no_alpha(bitmap, source, destination);
    }
    else
    {
        blit_bitmap_scaled_no_alpha(bitmap, source, destination);
    }
}

__flatten void Painter::clear(Color color)
{
    clear_rectangle(_bitmap->bound(), color);
}

__flatten void Painter::clear_rectangle(Rectangle rectangle, Color color)
{
    rectangle = apply_transform(rectangle);
    rectangle = apply_clip(rectangle);

    if (rectangle.is_empty())
    {
        return;
    }

    for (int x = 0; x < rectangle.width(); x++)
    {
        for (int y = 0; y < rectangle.height(); y++)
        {
            _bitmap->set_pixel_no_check(Vec2i(rectangle.x() + x, rectangle.y() + y), color);
        }
    }
}

__flatten void Painter::fill_rectangle(Rectangle rectangle, Color color)
{
    rectangle = apply_transform(rectangle);
    rectangle = apply_clip(rectangle);

    if (rectangle.is_empty())
    {
        return;
    }

    for (int x = 0; x < rectangle.width(); x++)
    {
        for (int y = 0; y < rectangle.height(); y++)
        {
            _bitmap->blend_pixel_no_check(Vec2i(rectangle.x() + x, rectangle.y() + y),
                                          color);
        }
    }
}

__flatten void Painter::fill_insets(Rectangle rectangle, Insets insets, Color color)
{
    Rectangle left_ear = rectangle.take_left(insets.left());
    Rectangle right_ear = rectangle.take_right(insets.right());

    Rectangle top = rectangle.cutoff_left_and_right(insets.left(), insets.right()).take_top(insets.top());
    Rectangle bottom = rectangle.cutoff_left_and_right(insets.left(), insets.right()).take_bottom(insets.bottom());

    fill_rectangle(left_ear, color);
    fill_rectangle(right_ear, color);
    fill_rectangle(top, color);
    fill_rectangle(bottom, color);
}

__flatten void Painter::fill_triangle(Vec2i p0, Vec2i p1, Vec2i p2, Color color)
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

    float dx1 = 0;
    float dx2 = 0;
    float dx3 = 0;

    if (b.y() - a.y() > 0)
    {
        dx1 = (b.x() - a.x()) / (float)(b.y() - a.y());
    }

    if (c.y() - a.y() > 0)
    {
        dx2 = (c.x() - a.x()) / (float)(c.y() - a.y());
    }

    if (c.y() - b.y() > 0)
    {
        dx3 = (c.x() - b.x()) / (float)(c.y() - b.y());
    }

    if (dx1 > dx2)
    {
        do
        {
            draw_line(Vec2i(s.x() - 1, s.y()), Vec2i(e.x() + 1, s.y()), color);

            s += Vec2f(dx2, 1);
            e += Vec2f(dx1, 1);
        } while (s.y() <= b.y());

        e = b;

        do
        {
            draw_line(Vec2i(s.x() - 1, s.y()), Vec2i(e.x() + 1, s.y()), color);

            s += Vec2f(dx2, 1);
            e += Vec2f(dx3, 1);
        } while (s.y() <= c.y());
    }
    else
    {
        do
        {
            draw_line(Vec2i(s.x() - 1, s.y()), Vec2i(e.x() + 1, s.y()), color);

            s += Vec2f(dx1, 1);
            e += Vec2f(dx2, 1);
        } while (s.y() <= b.y());

        s = b;

        do
        {
            draw_line(Vec2i(s.x() - 1, s.y()), Vec2i(e.x() + 1, s.y()), color);

            s += Vec2f(dx3, 1);
            e += Vec2f(dx2, 1);
        } while (s.y() <= c.y());
    }
}

static float sample_fill_circle(Vec2i center, float radius, Vec2i position)
{
    float distance = center.distance_to(position) - radius;
    return clamp(0.5 - distance, 0, 1);
}

static void fill_circle_helper(Painter &painter, Rectangle bound, Vec2i center, int radius, Color color)
{
    for (int x = 0; x < bound.width(); x++)
    {
        for (int y = 0; y < bound.height(); y++)
        {
            float distance = sample_fill_circle(center, radius - 0.5, Vec2i(x, y));
            float alpha = color.alphaf() * distance;

            painter.plot_pixel(Vec2i(bound.x() + x, bound.y() + y), color.with_alpha(alpha));
        }
    }
}

__flatten void Painter::fill_rounded_rectangle(Rectangle bound, int radius, Color color)
{
    radius = MIN(radius, bound.height() / 2);
    radius = MIN(radius, bound.width() / 2);

    Rectangle left_ear = bound.take_left(radius);
    Rectangle right_ear = bound.take_right(radius);

    fill_circle_helper(*this, left_ear.take_top(radius), Vec2i(radius - 1, radius - 1), radius, color);
    fill_circle_helper(*this, left_ear.take_bottom(radius), Vec2i(radius - 1, 0), radius, color);
    fill_rectangle(left_ear.cutoff_top_and_botton(radius, radius), color);

    fill_circle_helper(*this, right_ear.take_top(radius), Vec2i(0, radius - 1), radius, color);
    fill_circle_helper(*this, right_ear.take_bottom(radius), Vec2i::zero(), radius, color);
    fill_rectangle(right_ear.cutoff_top_and_botton(radius, radius), color);

    fill_rectangle(bound.cutoff_left_and_right(radius, radius), color);
}

__flatten void Painter::fill_checkboard(Rectangle bound, int cell_size, Color fg_color, Color bg_color)
{
    for (int x = 0; x < bound.width(); x++)
    {
        for (int y = 0; y < bound.height(); y++)
        {
            Vec2i position = bound.position() + Vec2i(x, y);

            if (((y / cell_size) * bound.width() + (x / cell_size) + (y / cell_size) % 2) % 2 == 0)
            {
                plot_pixel(position, fg_color);
            }
            else
            {
                plot_pixel(position, bg_color);
            }
        }
    }
}

void Painter::draw_line_x_aligned(int x, int start, int end, Color color)
{
    for (int i = start; i <= end; i++)
    {
        plot_pixel(Vec2i(x, i), color);
    }
}

void Painter::draw_line_y_aligned(int y, int start, int end, Color color)
{
    for (int i = start; i <= end; i++)
    {
        plot_pixel(Vec2i(i, y), color);
    }
}

void Painter::draw_line_not_aligned(Vec2i a, Vec2i b, Color color)
{
    int dx = abs(b.x() - a.x());
    int dy = abs(b.y() - a.y());

    int sx = a.x() < b.x() ? 1 : -1;
    int sy = a.y() < b.y() ? 1 : -1;

    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        plot_pixel(a, color);

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
#define __plot(__x, __y, __brightness) plot_pixel(Vec2i((__x), (__y)), color.with_alpha(color.alphaf() * (__brightness)))

__flatten void Painter::draw_line_antialias(Vec2i a, Vec2i b, Color color)
{
    double x0 = a.x();
    double y0 = a.y();
    double x1 = b.x();
    double y1 = b.y();

    const bool steep = abs(y1 - a.y()) > abs(x1 - x0);
    if (steep)
    {
        swap(x0, y0);
        swap(x1, y1);
    }
    if (x0 > x1)
    {
        swap(x0, x1);
        swap(y0, y1);
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

__flatten void Painter::draw_line(Vec2i a, Vec2i b, Color color)
{
    if (a.x() == b.x())
    {
        draw_line_x_aligned(a.x(), MIN(a.y(), b.y()), MAX(a.y(), b.y()), color);
    }
    else if (a.y() == b.y())
    {
        draw_line_y_aligned(a.y(), MIN(a.x(), b.x()), MAX(a.x(), b.x()), color);
    }
    else
    {
        draw_line_not_aligned(a, b, color);
    }
}

__flatten void Painter::draw_rectangle(Rectangle rect, Color color)
{
    Vec2i topleft = rect.position();
    Vec2i topright = rect.position() + rect.size().extract_x() - Vec2i::oneX();
    Vec2i bottomleft = rect.position() + rect.size().extract_y() - Vec2i::oneY();
    Vec2i bottomright = rect.position() + rect.size() - Vec2i::one();

    draw_line(topleft, topright, color);
    draw_line(bottomleft, bottomright, color);

    draw_line(topleft + Vec2i::oneY(), bottomleft - Vec2i::oneY(), color);
    draw_line(topright + Vec2i::oneY(), bottomright - Vec2i::oneY(), color);
}

__flatten void Painter::draw_triangle(Vec2i p0, Vec2i p1, Vec2i p2, Color color)
{
    draw_line(p0, p1, color);
    draw_line(p1, p2, color);
    draw_line(p2, p0, color);
}

static double sample_draw_circle(Vec2i center, double radius, double thickness, Vec2i position)
{
    double distance = abs(center.distance_to(position) - (radius - thickness / 2));

    return clamp((0.5 - distance) + (thickness / 2), 0, 1);
}

void Painter::draw_circle_helper(Rectangle bound, Vec2i center, int radius, int thickness, Color color)
{
    for (int x = 0; x < bound.width(); x++)
    {
        for (int y = 0; y < bound.height(); y++)
        {
            Vec2i position = Vec2i(x, y);

            double distance = sample_draw_circle(center, radius - 0.5, thickness, position);
            double alpha = color.alphaf() * distance;

            plot_pixel(bound.position() + position, color.with_alpha(alpha));
        }
    }
}

__flatten void Painter::draw_rounded_rectangle(Rectangle bound, int radius, int thickness, Color color)
{
    radius = MIN(radius, bound.height() / 2);
    radius = MIN(radius, bound.width() / 2);

    Rectangle left_ear = bound.take_left(radius);
    Rectangle right_ear = bound.take_right(radius);

    draw_circle_helper(left_ear.take_top(radius), Vec2i(radius - 1, radius - 1), radius, thickness, color);
    draw_circle_helper(left_ear.take_bottom(radius), Vec2i(radius - 1, 0), radius, thickness, color);

    draw_circle_helper(right_ear.take_top(radius), Vec2i(0, radius - 1), radius, thickness, color);
    draw_circle_helper(right_ear.take_bottom(radius), Vec2i::zero(), radius, thickness, color);

    fill_rectangle(left_ear.cutoff_top_and_botton(radius, radius).take_left(thickness), color);
    fill_rectangle(right_ear.cutoff_top_and_botton(radius, radius).take_right(thickness), color);

    fill_rectangle(bound.cutoff_left_and_right(radius, radius).take_top(thickness), color);
    fill_rectangle(bound.cutoff_left_and_right(radius, radius).take_bottom(thickness), color);
}

__flatten void Painter::blit_icon(Icon &icon, IconSize size, Rectangle destination, Color color)
{
    Bitmap &bitmap = *icon.bitmap(size);

    for (int x = 0; x < destination.width(); x++)
    {
        for (int y = 0; y < destination.height(); y++)
        {
            Vec2f sample_point(
                x / (double)destination.width(),
                y / (double)destination.height());

            Color sample = bitmap.sample(sample_point);

            auto alpha = sample.alphaf() * color.alphaf();
            plot_pixel(destination.position() + Vec2i(x, y), color.with_alpha(alpha));
        }
    }
}

__flatten void Painter::blur_rectangle(Rectangle rectangle, int radius)
{
    rectangle = apply_transform(rectangle);
    rectangle = apply_clip(rectangle);

    stackblurJob((unsigned char *)_bitmap->pixels(),
                 _bitmap->width(),
                 _bitmap->height(),
                 radius,
                 rectangle.x(), rectangle.x() + rectangle.width(),
                 rectangle.y(), rectangle.y() + rectangle.height());
}

__flatten void Painter::blit_bitmap_colored(Bitmap &bitmap, Rectangle source, Rectangle destination, Color color)
{
    for (int x = 0; x < destination.width(); x++)
    {
        for (int y = 0; y < destination.height(); y++)
        {
            Vec2f sample_point(
                x / (double)destination.width(),
                y / (double)destination.height());

            Color sample = bitmap.sample(source, sample_point);

            auto alpha = sample.redf() * color.alphaf();
            plot_pixel(destination.position() + Vec2i(x, y), color.with_alpha(alpha));
        }
    }
}

void Painter::draw_glyph(Font &font, Glyph &glyph, Vec2i position, Color color)
{
    Rectangle dest(position - glyph.origin, glyph.bound.size());
    blit_bitmap_colored(font.bitmap(), glyph.bound, dest, color);
}

__flatten void Painter::draw_string(Font &font, const char *str, Vec2i position, Color color)
{
    codepoint_foreach(reinterpret_cast<const uint8_t *>(str), [&](auto codepoint) {
        Glyph &glyph = font.glyph(codepoint);
        draw_glyph(font, glyph, position, color);
        position = position + Vec2i(glyph.advance, 0);
    });
}

void Painter::draw_string_within(Font &font, const char *str, Rectangle container, Position position, Color color)
{
    Rectangle bound = font.mesure_string(str);

    bound = bound.place_within(container, position);

    draw_string(font, str, Vec2i(bound.x(), bound.y() + bound.height() / 2 + 4), color);
}

void Painter::draw_truetype_glyph(TrueTypeFont *font, TrueTypeGlyph *glyph, Vec2i position, Color color)
{
    Rectangle dest(position + glyph->offset, glyph->bound.size());
    TrueTypeAtlas *atlas = truetypefont_get_atlas(font);

    for (int x = 0; x < dest.width(); x++)
    {
        for (int y = 0; y < dest.height(); y++)
        {
            int pixel_index = (y + glyph->bound.y()) * atlas->width + (x + glyph->bound.x());

            auto alpha = (atlas->buffer[pixel_index] / 255.0) * color.alphaf();

            plot_pixel(dest.position() + Vec2i(x, y), color.with_alpha(alpha));
        }
    }

    //draw_rectangle( dest, COLOR_RED);
}

#include <libsystem/Logger.h>

__flatten void Painter::draw_truetype_string(TrueTypeFont *font, const char *string, Vec2i position, Color color)
{
    // Rectangle bound = truetypefont_mesure_string(font, string);
    // TrueTypeFontMetrics metrics = truetypefont_get_metrics(font);
    //
    // bound = rectangle_offset(bound, position);
    // bound = rectangle_offset(bound, Vec2i(0, -metrics.ascent));
    //
    // draw_rectangle( bound, COLOR_GREEN);

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

            draw_truetype_glyph(font, glyph, position, color);

            position = position + Vec2i(glyph->advance, 0);
        }

        previouse = codepoint;

        size_t size = utf8_to_codepoint((const uint8_t *)string, &codepoint);
        string += size;
    }
}

void Painter::draw_truetype_string_within(TrueTypeFont *font, const char *str, Rectangle container, Position position, Color color)
{
    Rectangle bound =
        truetypefont_mesure_string(font, str)
            .place_within(container, position);

    draw_truetype_string(

        font,
        str,
        Vec2i(bound.x(), bound.y() + bound.height() / 2 + 4),
        color);
}
