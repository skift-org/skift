#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/Bitmap.h>
#include <libgraphic/Font.h>

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

void painter_blit_icon(Painter *painter, Bitmap *icon, Rectangle destination, Color color);

void painter_clear(Painter *painter, Color color);

void painter_clear_rectangle(Painter *painter, Rectangle rectangle, Color color);

void painter_fill_rectangle(Painter *painter, Rectangle rectangle, Color color);

void painter_fill_triangle(Painter *painter, Vec2i p0, Vec2i p1, Vec2i p2, Color color);

void painter_draw_line(Painter *painter, Vec2i from, Vec2i to, Color color);

void painter_draw_rectangle(Painter *painter, Rectangle rectangle, Color color);

void painter_draw_triangle(Painter *painter, Vec2i p0, Vec2i p1, Vec2i p2, Color color);

void painter_draw_line(Painter *painter, Vec2i a, Vec2i b, Color color);

void painter_draw_glyph(Painter *painter, Font *font, Glyph *glyph, Vec2i position, Color color);

void painter_draw_string(Painter *painter, Font *font, const char *str, Vec2i position, Color color);
