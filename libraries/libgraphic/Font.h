#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/Bitmap.h>
#include <libsystem/unicode/Codepoint.h>

typedef struct
{
    Codepoint codepoint;
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

bool font_has_glyph(Font *font, Codepoint codepoint);

Glyph *font_glyph(Font *this, Codepoint codepoint);
