#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/bitmap.h>
#include <libgraphic/font.h>

typedef struct
{
    bitmap_t *bitmap;

    rectangle_t cliprect;
    int cliprect_stack_top;
    rectangle_t cliprect_stack[32];
} painter_t;

painter_t *painter(bitmap_t *bmp);

void painter_push_cliprect(painter_t *paint, rectangle_t cliprect);

void painter_pop_cliprect(painter_t *paint);

void painter_plot_pixel(painter_t *painter, point_t position, color_t color);

void painter_blit_bitmap(painter_t *paint, bitmap_t *src, rectangle_t src_rect, rectangle_t dst_rect);

void painter_clear(painter_t *paint, color_t color);

void painter_clear_rect(painter_t *paint, rectangle_t rect, color_t color);

void painter_fill_rect(painter_t *paint, rectangle_t rect, color_t color);

// TODO: void painter_fill_circle(painter_t* paint, ...);

// TODO: void painter_fill_triangle(painter_t* paint, ...);

void painter_draw_line(painter_t *paint, point_t a, point_t b, color_t color);

void painter_draw_rect(painter_t *paint, rectangle_t rect, color_t color);

// TODO: void painter_draw_circle(painter_t* paint, ...);

// TODO: void painter_draw_triangle(painter_t* paint, ...);

// void painter_draw_text(painter_t *paint, const char *text, point_t position, color_t color);

void painter_draw_glyph(painter_t *paint, font_t *font, glyph_t *glyph, point_t position, float size, color_t color);

void painter_draw_text(
    painter_t *paint,
    font_t *font,
    const char *text,
    int text_size,
    point_t position,
    float font_size,
    color_t color);
