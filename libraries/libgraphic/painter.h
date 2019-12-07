#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/bitmap.h>
#include <libgraphic/font.h>

typedef struct
{
    Bitmap *bitmap;

    rectangle_t cliprect;
    int cliprect_stack_top;
    rectangle_t cliprect_stack[32];
} Painter;

Painter *painter_create(Bitmap *bmp);

void painter_destroy(Painter *this);

void painter_push_cliprect(Painter *paint, rectangle_t cliprect);

void painter_pop_cliprect(Painter *paint);

void painter_plot_pixel(Painter *painter, point_t position, color_t color);

void painter_blit_bitmap(Painter *paint, Bitmap *src, rectangle_t src_rect, rectangle_t dst_rect);

void painter_clear(Painter *paint, color_t color);

void painter_clear_rect(Painter *paint, rectangle_t rect, color_t color);

void painter_fill_rect(Painter *paint, rectangle_t rect, color_t color);

// TODO: void painter_fill_circle(Painter* paint, ...);

// TODO: void painter_fill_triangle(Painter* paint, ...);

void painter_draw_line(Painter *paint, point_t a, point_t b, color_t color);

void painter_draw_rect(Painter *paint, rectangle_t rect, color_t color);

// TODO: void painter_draw_circle(Painter* paint, ...);

// TODO: void painter_draw_triangle(Painter* paint, ...);

// void painter_draw_text(Painter *paint, const char *text, point_t position, color_t color);

void painter_draw_glyph(Painter *paint, font_t *font, glyph_t *glyph, point_t position, float size, color_t color);

void painter_draw_text(
    Painter *paint,
    font_t *font,
    const char *text,
    int text_size,
    point_t position,
    float font_size,
    color_t color);
