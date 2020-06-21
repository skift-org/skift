#pragma once

#include <libgraphic/Bitmap.h>
#include <libsystem/unicode/Codepoint.h>

typedef struct TrueTypeFamily TrueTypeFamily;

typedef struct TrueTypeFont TrueTypeFont;

typedef struct
{
    int ascent;
    int descent;
    int linegap;
} TrueTypeFontMetrics;

typedef struct TrueTypeGlyph
{
    Codepoint codepoint;
    Rectangle bound;
    Vec2i offset;
    int advance;
} TrueTypeGlyph;

typedef struct TrueTypeAtlas
{
    int width;
    int height;
    uint8_t buffer[];
} TrueTypeAtlas;

TrueTypeFamily *truetype_family_create(const char *path);

void truetype_family_destroy(TrueTypeFamily *family);

TrueTypeFont *truetypefont_create(TrueTypeFamily *family, int size);

void truetypefont_raster_range(TrueTypeFont *font, Codepoint start, Codepoint end);

TrueTypeAtlas *truetypefont_get_atlas(TrueTypeFont *font);

TrueTypeGlyph *truetypefont_get_glyph_for_codepoint(TrueTypeFont *font, Codepoint codepoint);

Rectangle truetypefont_mesure_string(TrueTypeFont *font, const char *string);

int truetypefont_get_kerning_for_codepoints(TrueTypeFont *font, Codepoint left, Codepoint right);

TrueTypeFontMetrics truetypefont_get_metrics(TrueTypeFont *font);
