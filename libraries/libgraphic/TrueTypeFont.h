#pragma once

#include <libgraphic/Bitmap.h>

typedef struct TrueTypeFamily TrueTypeFamily;

typedef struct TrueTypeFont TrueTypeFont;

typedef struct TrueTypeGlyph TrueTypeGlyph;

TrueTypeFamily *truetype_family_create(const char *path);

void truetype_family_destroy(TrueTypeFamily *family);

TrueTypeFont *truetypefont_create(TrueTypeFamily *family, int size);
