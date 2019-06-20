#pragma once

#include <skift/bitmap.h>

typedef struct
{
    int codepoint;
    rectangle_t bound;
    point_t origin;
    int advance;
} glyph_t;

typedef struct
{
    bitmap_t* bitmap;

    glyph_t default_glyph;
    glyph_t* glyph;
} font_t;

font_t* font(const char* name);

void font_delete(font_t* this);

glyph_t* font_glyph(font_t *this, int codepoint);

int font_measure_width(font_t* this, float font_size, const char* str, int str_size);
