#pragma once

#include <libmath/Mat3x2.h>
#include <libutils/Array.h>

#include <libgraphic/Font.h>
#include <libgraphic/Icon.h>

#include <libgraphic/Bitmap.h>
#include <libgraphic/rast/Rasterizer.h>
#include <libgraphic/rast/TransformStack.h>

namespace Graphic
{

struct Painter
{
private:
    Bitmap &_bitmap;
    TransformStack _stack;
    Rasterizer _rasterizer;

public:
    Bitmap &bitmap() const
    {
        return _rasterizer.bitmap();
    }

    Painter(Bitmap &bitmap)
        : _bitmap{bitmap},
          _stack{bitmap.bound()},
          _rasterizer{_bitmap, _stack}
    {
    }

    /* --- context ---------------------------------------------------------- */

    void push() { _stack.push(); }
    void pop() { _stack.pop(); }
    void clip(Math::Recti rectangle) { _stack.clip(rectangle); }
    void transform(Math::Vec2i offset) { _stack.origin(offset); }

    /* --- Drawing ---------------------------------------------------------- */

    void plot(Math::Vec2i position, Color color);

    void blit(Bitmap &bitmap, Math::Recti source, Math::Recti destination);
    void blit(Bitmap &bitmap, BitmapScaling scaling, Math::Recti destionation);
    void blit(Icon &icon, IconSize size, Math::Recti destination, Color color);
    void blit_rounded(Bitmap &bitmap, Math::Recti source, Math::Recti destination, int radius);

    void clear(Color color);
    void clear(Math::Recti rectangle, Color color);

    void fill(const Path &path, const Math::Mat3x2f &transform, Paint paint);
    void fill(const EdgeList &edges, const Math::Mat3x2f &transform, Paint paint);

    void fill_rectangle(Math::Recti rectangle, Color color);
    void fill_insets(Math::Recti rectangle, Insetsi insets, Color color);
    void fill_rectangle_rounded(Math::Recti bound, int radius, Color color);
    void fill_checkboard(Math::Recti bound, int cell_size, Color fg_color, Color bg_color);

    void draw_line(Math::Vec2i from, Math::Vec2i to, Color color);
    void draw_rectangle(Math::Recti rectangle, Color color);
    void draw_triangle(Math::Vec2i p0, Math::Vec2i p1, Math::Vec2i p2, Color color);
    void draw_rectangle_rounded(Math::Recti bound, int radius, int thickness, Color color);
    void draw_glyph(Font &font, const Glyph &glyph, Math::Vec2i position, Color color);
    void draw_string(Font &font, const char *str, Math::Vec2i position, Color color);
    void draw_string_within(Font &font, const char *str, Math::Recti container, Math::Anchor anchor, Color color);

    /* --- Effects ---------------------------------------------------------- */

    void blur(Math::Recti rectangle, int radius);
    void saturation(Math::Recti rectangle, float value);
    void noise(Math::Recti rectangle, float opacity);
    void acrylic(Math::Recti rectangle);
    void sepia(Math::Recti rectangle, float value);
    void tint(Math::Recti rectangle, Color color);

private:
    void blit_fast(Bitmap &bitmap, Math::Recti source, Math::Recti destination);
    void blit_scaled(Bitmap &bitmap, Math::Recti source, Math::Recti destination);
    void blit_colored(Bitmap &src, Math::Recti source, Math::Recti destination, Color color);
    void blit_circle_helper(Bitmap &bitmap, Math::Recti source, Math::Recti destination, Math::Vec2i center, int radius);

    void fill_circle_helper(Math::Recti bound, Math::Vec2i center, int radius, Color color);

    void draw_circle_helper(Math::Recti bound, Math::Vec2i center, int radius, int thickness, Color color);
};

} // namespace Graphic