#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Font.h>
#include <libgraphic/Icon.h>
#include <libmath/Mat3x2.h>
#include <libutils/Array.h>

#define STATESTACK_SIZE 32

namespace Graphic
{

struct PainterState
{
    Math::Vec2i origin;
    Math::Recti clip;
};

struct SourceDestionation
{
    Math::Recti source;
    Math::Recti destination;

    bool is_empty()
    {
        return destination.is_empty();
    }
};

class Painter
{
private:
    RefPtr<Bitmap> _bitmap;
    int _state_stack_top = 0;
    Array<PainterState, STATESTACK_SIZE> _state_stack;

public:
    RefPtr<Bitmap> bitmap() const { return _bitmap; }

    Painter(RefPtr<Bitmap> bitmap);

    /* --- context ---------------------------------------------------------- */

    void push();

    void pop();

    void clip(Math::Recti rectangle);

    void transform(Math::Vec2i offset);

    Math::Recti apply(Math::Recti rectangle);

    SourceDestionation apply(Math::Recti source, Math::Recti destination);

    /* --- Drawing ---------------------------------------------------------- */

    void plot(Math::Vec2i position, Color color);

    void blit(Bitmap &bitmap, Math::Recti source, Math::Recti destination);

    void blit(Bitmap &bitmap, BitmapScaling scaling, Math::Recti destionation);

    void blit(Icon &icon, IconSize size, Math::Recti destination, Color color);

    void blit_rounded(Bitmap &bitmap, Math::Recti source, Math::Recti destination, int radius);

    void clear(Color color);

    void clear(Math::Recti rectangle, Color color);

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

    void draw_string_within(Font &font, const char *str, Math::Recti container, Anchor anchor, Color color);

    /* --- Effects ---------------------------------------------------------- */

    void blur(Math::Recti rectangle, int radius);

    void saturation(Math::Recti rectangle, float value);

    void noise(Math::Recti rectangle, float opacity);

    void acrylic(Math::Recti rectangle);

    void sepia(Math::Recti rectangle, float value);

    void tint(Math::Recti rectangle, Color color);

    Math::Recti clip() const
    {
        return _state_stack[_state_stack_top].clip;
    }

private:
    Math::Vec2i origin() const { return _state_stack[_state_stack_top].origin; };

    Math::Recti apply_clip(Math::Recti rectangle);

    Math::Recti apply_transform(Math::Recti rectangle);

    void blit_fast(Bitmap &bitmap, Math::Recti source, Math::Recti destination);

    void blit_scaled(Bitmap &bitmap, Math::Recti source, Math::Recti destination);

    void blit_colored(Bitmap &src, Math::Recti source, Math::Recti destination, Color color);

    void draw_circle_helper(Math::Recti bound, Math::Vec2i center, int radius, int thickness, Color color);
};

} // namespace Graphic