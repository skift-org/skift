#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/bitmap.h>

typedef struct
{
    int codepoint;
    Rectangle bound;
    Point origin;
    int advance;
} glyph_t;

typedef struct
{
    Bitmap *bitmap;

    glyph_t default_glyph;
    glyph_t *glyph;
} font_t;

font_t *font(const char *name);

glyph_t *font_glyph(font_t *this, int codepoint);

int font_measure_width(font_t *this, float font_size, const char *str, int str_size);
