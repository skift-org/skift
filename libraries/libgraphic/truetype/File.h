#pragma once

#include <libsystem/utils/Slice.h>

namespace truetype
{

struct File
{
    unsigned char *data; // pointer to .ttf file

    int numGlyphs; // number of glyphs, needed for range checking

    int loca, head, glyf, hhea, hmtx, kern, gpos; // table locations as offset from start of .ttf
    int index_map;                                // a cmap mapping for our chosen character encoding
    int indexToLocFormat;                         // format needed to map from glyph index to glyph

    Slice cff;         // cff font data
    Slice charstrings; // the charstring index
    Slice gsubrs;      // global charstring subroutines index
    Slice subrs;       // private charstring subroutines index
    Slice fontdicts;   // array of font dicts
    Slice fdselect;    // map from glyph to fontdict
};

} // namespace truetype
