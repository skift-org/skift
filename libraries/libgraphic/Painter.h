#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/Bitmap.h>
#include <libgraphic/Font.h>

typedef struct Painter
{
    Bitmap *bitmap;

    Rectangle cliprect;
    int cliprect_stack_top;
    Rectangle cliprect_stack[32];
} Painter;

Painter *painter_create(Bitmap *bitmap);

void painter_destroy(Painter *painter);

void painter_push_cliprect(Painter *painter, Rectangle rectangle);

void painter_pop_cliprect(Painter *painter);

void painter_plot_pixel(Painter *painter, Point position, Color color);

void painter_blit_bitmap(Painter *painter, Bitmap *bitmap, Rectangle source, Rectangle destination);

void painter_clear(Painter *painter, Color color);

void painter_clear_rectangle(Painter *painter, Rectangle rectangle, Color color);

void painter_fill_rectangle(Painter *painter, Rectangle rectangle, Color color);

void painter_draw_rectangle(Painter *painter, Rectangle rectangle, Color color);

void painter_draw_line(Painter *painter, Point from, Point to, Color color);

void painter_draw_glyph(Painter *painter, Font *font, Glyph *glyph, Point position, Color color);
