#pragma once

#include <libgraphic/Bitmap.h>
#include <libsystem/unicode/Codepoint.h>

typedef struct TrueTypeFamily TrueTypeFamily;

typedef struct TrueTypeFont TrueTypeFont;

typedef struct TrueTypeGlyph TrueTypeGlyph;

TrueTypeFamily *truetype_family_create(const char *path);

void truetype_family_destroy(TrueTypeFamily *family);

TrueTypeFont *truetypefont_create(TrueTypeFamily *family, int size);

void truetypefont_raster_range(TrueTypeFont *font, Codepoint start, Codepoint end);
