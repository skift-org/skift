#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Font.h>
#include <libgraphic/Icon.h>
#include <libgraphic/TrueTypeFont.h>

#define CLIPSTACK_SIZE 32
#define ORIGINSTACK_SIZE 32

typedef struct Painter
{
    Bitmap *bitmap;

    int clipstack_top;
    Rectangle clipstack[CLIPSTACK_SIZE];

    int originestack_top;
    Vec2i originestack[ORIGINSTACK_SIZE];
} Painter;

Painter *painter_create(Bitmap *bitmap);

void painter_destroy(Painter *painter);

void painter_push_clip(Painter *painter, Rectangle clip);

void painter_pop_clip(Painter *painter);

void painter_push_origin(Painter *painter, Vec2i origin);

void painter_pop_origin(Painter *painter);

void painter_plot_pixel(Painter *painter, Vec2i position, Color color);

void painter_blit_bitmap(Painter *painter, Bitmap *bitmap, Rectangle source, Rectangle destination);

void painter_blit_bitmap_no_alpha(Painter *painter, Bitmap *bitmap, Rectangle source, Rectangle destination);

void painter_blit_icon(Painter *painter, Icon *icon, IconSize size, Rectangle destination, Color color);

void painter_clear(Painter *painter, Color color);

void painter_clear_rectangle(Painter *painter, Rectangle rectangle, Color color);

void painter_fill_rectangle(Painter *painter, Rectangle rectangle, Color color);

void painter_fill_insets(Painter *painter, Rectangle rectangle, Insets insets, Color color);

void painter_fill_triangle(Painter *painter, Vec2i p0, Vec2i p1, Vec2i p2, Color color);

void painter_fill_rounded_rectangle(Painter *painter, Rectangle bound, int radius, Color color);

void painter_fill_checkboard(Painter *painter, Rectangle bound, int cell_size, Color fg_color, Color bg_color);

void painter_draw_line(Painter *painter, Vec2i from, Vec2i to, Color color);

void painter_draw_line_antialias(Painter *painter, Vec2i a, Vec2i b, Color color);

void painter_draw_rectangle(Painter *painter, Rectangle rectangle, Color color);

void painter_draw_triangle(Painter *painter, Vec2i p0, Vec2i p1, Vec2i p2, Color color);

void painter_draw_line(Painter *painter, Vec2i a, Vec2i b, Color color);

void painter_draw_circle_helper(Painter *painter, Rectangle bound, Vec2i center, int radius, int thickness, Color color);

void painter_draw_rounded_rectangle(Painter *painter, Rectangle bound, int radius, int thickness, Color color);

void painter_blur_rectangle(Painter *painter, Rectangle rectangle, int radius);

void painter_draw_glyph(Painter *painter, Font *font, Glyph *glyph, Vec2i position, Color color);

void painter_draw_string(Painter *painter, Font *font, const char *str, Vec2i position, Color color);

void painter_draw_string_within(
    Painter *painter,
    Font *font,
    const char *str,
    Rectangle container,
    Position position,
    Color color);

void painter_draw_truetype_string(Painter *painter, TrueTypeFont *font, const char *string, Vec2i position, Color color);

void painter_draw_truetype_string_within(Painter *painter, TrueTypeFont *font, const char *str, Rectangle container, Position position, Color color);
