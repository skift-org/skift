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
} Glyph;

typedef struct
{
    Bitmap *bitmap;

    Glyph default_glyph;
    Glyph *glyph;
} Font;

Font *font_create(const char *name);

void font_destroy(Font *this);

bool font_has_glyph(Font *font, int codepoint);

Glyph *font_glyph(Font *this, int codepoint);

int font_measure_width(Font *this, float font_size, const char *str, int str_size);
