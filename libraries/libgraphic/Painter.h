#pragma once

#include <libutils/Trans2.h>

#include <libgraphic/Bitmap.h>
#include <libgraphic/Font.h>
#include <libgraphic/Icon.h>
#include <libgraphic/vector/Path.h>

#define STATESTACK_SIZE 32

struct PainterState
{
    Vec2i origin;
    Recti clip;
};

struct SourceDestionation
{
    Recti source;
    Recti destination;

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
    PainterState _state_stack[STATESTACK_SIZE];

public:
    Painter(RefPtr<Bitmap> bitmap);

    /* --- context ---------------------------------------------------------- */

    void push();

    void pop();

    void clip(Recti rectangle);

    void transform(Vec2i offset);

    Recti apply(Recti rectangle);

    SourceDestionation apply(Recti source, Recti destination);

    /* --- Drawing ---------------------------------------------------------- */

    void plot(Vec2i position, Color color);

    void blit(Bitmap &bitmap, Recti source, Recti destination);

    void blit(Bitmap &bitmap, BitmapScaling scaling, Recti destionation);

    void blit_no_alpha(Bitmap &bitmap, Recti source, Recti destination);

    void blit(Icon &icon, IconSize size, Recti destination, Color color);

    void blit_rounded(Bitmap &bitmap, Recti source, Recti destination, int radius);

    void blit_rounded_no_alpha(Bitmap &bitmap, Recti source, Recti destination, int radius);

    void clear(Color color);

    void clear(Recti rectangle, Color color);

    void fill_rectangle(Recti rectangle, Color color);

    void fill_insets(Recti rectangle, Insetsi insets, Color color);

    void fill_rectangle_rounded(Recti bound, int radius, Color color);

    void fill_checkboard(Recti bound, int cell_size, Color fg_color, Color bg_color);

    void draw_line(Vec2i from, Vec2i to, Color color);

    void draw_rectangle(Recti rectangle, Color color);

    void draw_triangle(Vec2i p0, Vec2i p1, Vec2i p2, Color color);

    void draw_path(const graphic::Path &path, Vec2f pos, Trans2f transform, Color color);

    void draw_rectangle_rounded(Recti bound, int radius, int thickness, Color color);

    void draw_glyph(Font &font, const Glyph &glyph, Vec2i position, Color color);

    void draw_string(Font &font, const char *str, Vec2i position, Color color);

    void draw_string_within(Font &font, const char *str, Recti container, Anchor anchor, Color color);

    /* --- Effects ---------------------------------------------------------- */

    void blur(Recti rectangle, int radius);

    void saturation(Recti rectangle, float value);

    void noise(Recti rectangle, float opacity);

    void acrylic(Recti rectangle);

    void sepia(Recti rectangle, float value);

    void tint(Recti rectangle, Color color);

    Recti clip() const
    {
        return _state_stack[_state_stack_top].clip;
    }

private:
    Vec2i origin() const { return _state_stack[_state_stack_top].origin; };

    Recti apply_clip(Recti rectangle);

    Recti apply_transform(Recti rectangle);

    void blit_fast(Bitmap &bitmap, Recti source, Recti destination);

    void blit_scaled(Bitmap &bitmap, Recti source, Recti destination);

    void blit_fast_no_alpha(Bitmap &bitmap, Recti source, Recti destination);

    void blit_scaled_no_alpha(Bitmap &bitmap, Recti source, Recti destination);

    void blit_colored(Bitmap &src, Recti source, Recti destination, Color color);

    void draw_circle_helper(Recti bound, Vec2i center, int radius, int thickness, Color color);
};
