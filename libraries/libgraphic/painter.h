#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/bitmap.h>
#include <libgraphic/font.h>

typedef struct
{
    Bitmap *bitmap;

    Rectangle cliprect;
    int cliprect_stack_top;
    Rectangle cliprect_stack[32];
} Painter;

Painter *painter_create(Bitmap *bmp);

void painter_destroy(Painter *this);

void painter_push_cliprect(Painter *paint, Rectangle cliprect);

void painter_pop_cliprect(Painter *paint);

void painter_plot_pixel(Painter *painter, Point position, Color color);

void painter_blit_bitmap(Painter *paint, Bitmap *src, Rectangle src_rect, Rectangle dst_rect);

void painter_clear(Painter *paint, Color color);

void painter_clear_rect(Painter *paint, Rectangle rect, Color color);

void painter_fill_rect(Painter *paint, Rectangle rect, Color color);

// TODO: void painter_fill_circle(Painter* paint, ...);

// TODO: void painter_fill_triangle(Painter* paint, ...);

void painter_draw_line(Painter *paint, Point a, Point b, Color color);

void painter_draw_rect(Painter *paint, Rectangle rect, Color color);

// TODO: void painter_draw_circle(Painter* paint, ...);

// TODO: void painter_draw_triangle(Painter* paint, ...);

// void painter_draw_text(Painter *paint, const char *text, Point position, Color color);

void painter_draw_glyph(Painter *paint, font_t *font, glyph_t *glyph, Point position, float size, Color color);

void painter_draw_text(
    Painter *paint,
    font_t *font,
    const char *text,
    int text_size,
    Point position,
    float font_size,
    Color color);
