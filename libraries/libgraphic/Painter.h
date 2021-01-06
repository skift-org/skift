#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Font.h>
#include <libgraphic/Icon.h>
#include <libgraphic/vector/Path.h>
#include <libsystem/algebra/Trans2.h>

#define STATESTACK_SIZE 32

struct PainterState
{
    Vec2i origine;
    Recti clip;
};

class Painter
{
private:
    RefPtr<Bitmap> _bitmap;
    int _state_stack_top = 0;
    PainterState _state_stack[STATESTACK_SIZE];

public:
    Painter(RefPtr<Bitmap> bitmap);

    void push();

    void pop();

    void clip(Recti rectangle);

    void transform(Vec2i offset);

    void plot_pixel(Vec2i position, Color color);

    void blit_bitmap(Bitmap &bitmap, Recti source, Recti destination);

    void blit_bitmap_no_alpha(Bitmap &bitmap, Recti source, Recti destination);

    void blit_icon(Icon &icon, IconSize size, Recti destination, Color color);

    void clear(Color color);

    void clear_rectangle(Recti rectangle, Color color);

    void fill_rectangle(Recti rectangle, Color color);

    void fill_insets(Recti rectangle, Insetsi insets, Color color);

    void fill_triangle(Vec2i p0, Vec2i p1, Vec2i p2, Color color);

    void fill_rounded_rectangle(Recti bound, int radius, Color color);

    void fill_checkboard(Recti bound, int cell_size, Color fg_color, Color bg_color);

    void draw_line(Vec2i from, Vec2i to, Color color);

    void draw_line_antialias(Vec2i from, Vec2i to, Color color);

    void draw_rectangle(Recti rectangle, Color color);

    void draw_triangle(Vec2i p0, Vec2i p1, Vec2i p2, Color color);

    void draw_path(const graphic::Path &path, Vec2f pos, Trans2f transform, Color color);

    void draw_rounded_rectangle(Recti bound, int radius, int thickness, Color color);

    void draw_glyph(Font &font, Glyph &glyph, Vec2i position, Color color);

    void draw_string(Font &font, const char *str, Vec2i position, Color color);

    void draw_string_within(Font &font, const char *str, Recti container, Anchor anchor, Color color);

    void blur_rectangle(Recti rectangle, int radius);

    void saturation(Recti rectangle, float value);

private:
    Recti clip()
    {
        return _state_stack[_state_stack_top].clip;
    }

    Vec2i origine() { return _state_stack[_state_stack_top].origine; };

    Recti apply_clip(Recti rectangle);

    Recti apply_transform(Recti rectangle);

    void blit_bitmap_fast(Bitmap &bitmap, Recti source, Recti destination);

    void blit_bitmap_scaled(Bitmap &bitmap, Recti source, Recti destination);

    void blit_bitmap_fast_no_alpha(Bitmap &bitmap, Recti source, Recti destination);

    void blit_bitmap_scaled_no_alpha(Bitmap &bitmap, Recti source, Recti destination);

    void draw_line_x_aligned(int x, int start, int end, Color color);

    void draw_line_y_aligned(int y, int start, int end, Color color);

    void draw_line_not_aligned(Vec2i a, Vec2i b, Color color);

    void blit_bitmap_colored(Bitmap &src, Recti src_rect, Recti dst_rect, Color color);

    void draw_circle_helper(Recti bound, Vec2i center, int radius, int thickness, Color color);
};
