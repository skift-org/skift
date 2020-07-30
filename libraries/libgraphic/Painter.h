#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Font.h>
#include <libgraphic/Icon.h>
#include <libgraphic/TrueTypeFont.h>

#define STATESTACK_SIZE 32

struct PainterState
{
    Vec2i origine;
    Rectangle clip;
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

    void clip(Rectangle rectangle);

    void transform(Vec2i offset);

    void plot_pixel(Vec2i position, Color color);

    void blit_bitmap(Bitmap &bitmap, Rectangle source, Rectangle destination);

    void blit_bitmap_no_alpha(Bitmap &bitmap, Rectangle source, Rectangle destination);

    void blit_icon(Icon &icon, IconSize size, Rectangle destination, Color color);

    void clear(Color color);

    void clear_rectangle(Rectangle rectangle, Color color);

    void fill_rectangle(Rectangle rectangle, Color color);

    void fill_insets(Rectangle rectangle, Insets insets, Color color);

    void fill_triangle(Vec2i p0, Vec2i p1, Vec2i p2, Color color);

    void fill_rounded_rectangle(Rectangle bound, int radius, Color color);

    void fill_checkboard(Rectangle bound, int cell_size, Color fg_color, Color bg_color);

    void draw_line(Vec2i from, Vec2i to, Color color);

    void draw_line_antialias(Vec2i from, Vec2i to, Color color);

    void draw_rectangle(Rectangle rectangle, Color color);

    void draw_triangle(Vec2i p0, Vec2i p1, Vec2i p2, Color color);

    void draw_rounded_rectangle(Rectangle bound, int radius, int thickness, Color color);

    void blur_rectangle(Rectangle rectangle, int radius);

    void draw_glyph(Font &font, Glyph &glyph, Vec2i position, Color color);

    void draw_string(Font &font, const char *str, Vec2i position, Color color);

    void draw_string_within(Font &font, const char *str, Rectangle container, Position position, Color color);

    void draw_truetype_glyph(TrueTypeFont *font, TrueTypeGlyph *glyph, Vec2i position, Color color);

    void draw_truetype_string(TrueTypeFont *font, const char *string, Vec2i position, Color color);

    void draw_truetype_string_within(TrueTypeFont *font, const char *str, Rectangle container, Position position, Color color);

private:
    Rectangle clip() { return _state_stack[_state_stack_top].clip; }

    Vec2i origine() { return _state_stack[_state_stack_top].origine; };

    Rectangle apply_clip(Rectangle rectangle);

    Rectangle apply_transform(Rectangle rectangle);

    void blit_bitmap_fast(Bitmap &bitmap, Rectangle source, Rectangle destination);

    void blit_bitmap_scaled(Bitmap &bitmap, Rectangle source, Rectangle destination);

    void blit_bitmap_fast_no_alpha(Bitmap &bitmap, Rectangle source, Rectangle destination);

    void blit_bitmap_scaled_no_alpha(Bitmap &bitmap, Rectangle source, Rectangle destination);

    void draw_line_x_aligned(int x, int start, int end, Color color);

    void draw_line_y_aligned(int y, int start, int end, Color color);

    void draw_line_not_aligned(Vec2i a, Vec2i b, Color color);

    void blit_bitmap_colored(Bitmap &src, Rectangle src_rect, Rectangle dst_rect, Color color);

    void draw_circle_helper(Rectangle bound, Vec2i center, int radius, int thickness, Color color);
};
