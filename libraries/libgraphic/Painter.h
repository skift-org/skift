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
    Point originestack[ORIGINSTACK_SIZE];
} Painter;

Painter *painter_create(Bitmap *bitmap);

void painter_destroy(Painter *painter);

void painter_push_clip(Painter *painter, Rectangle clip);

void painter_pop_clip(Painter *painter);

void painter_push_origin(Painter *painter, Point origin);

void painter_pop_origin(Painter *painter);

void painter_plot_pixel(Painter *painter, Point position, Color color);

void painter_blit_bitmap(Painter *painter, Bitmap *bitmap, Rectangle source, Rectangle destination);

void painter_clear(Painter *painter, Color color);

void painter_clear_rectangle(Painter *painter, Rectangle rectangle, Color color);

void painter_fill_rectangle(Painter *painter, Rectangle rectangle, Color color);

void painter_draw_rectangle(Painter *painter, Rectangle rectangle, Color color);

void painter_draw_line(Painter *painter, Point from, Point to, Color color);

void painter_draw_glyph(Painter *painter, Font *font, Glyph *glyph, Point position, Color color);
