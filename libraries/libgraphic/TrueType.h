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

int truetype_isfont(uint8_t *font);

//////////////////////////////////////////////////////////////////////////////
//
// NEW TEXTURE BAKING API
//
// This provides options for packing multiple fonts into one atlas, not
// perfectly but better than nothing.

typedef struct
{
    unsigned short x0, y0, x1, y1; // coordinates of bbox in bitmap
    float xoff, yoff, xadvance;
    float xoff2, yoff2;
} truetype_packedchar;

typedef struct truetype_pack_context truetype_pack_context;

typedef struct stbrp_rect stbrp_rect;

int truetype_PackBegin(truetype_pack_context *spc, unsigned char *pixels, int width, int height, int stride_in_bytes, int padding);
// Initializes a packing context stored in the passed-in truetype_pack_context.
// Future calls using this context will pack characters into the bitmap passed
// in here: a 1-channel bitmap that is width * height. stride_in_bytes is
// the distance from one row to the next (or 0 to mean they are packed tightly
// together). "padding" is the amount of padding to leave between each
// character (normally you want '1' for bitmaps you'll use as textures with
// bilinear filtering).
//
// Returns 0 on failure, 1 on success.

void truetype_PackEnd(truetype_pack_context *spc);
// Cleans up the packing context and frees all memory.

#define TRUETYPE_POINT_SIZE(x) (-(x))

int truetype_PackFontRange(truetype_pack_context *spc, const unsigned char *fontdata, float font_size,
                           int first_unicode_char_in_range, int num_chars_in_range, truetype_packedchar *chardata_for_range);
// Creates character bitmaps from the font_index'th font found in fontdata (use
// font_index=0 if you don't know what that is). It creates num_chars_in_range
// bitmaps for characters with unicode values starting at first_unicode_char_in_range
// and increasing. Data for how to render them is stored in chardata_for_range;
// pass these to truetype_GetPackedQuad to get back renderable quads.
//
// font_size is the full height of the character from ascender to descender,
// as computed by truetype_ScaleForPixelHeight. To use a point size as computed
// by truetype_ScaleForMappingEmToPixels, wrap the point size in TRUETYPE_POINT_SIZE()
// and pass that result as 'font_size':
//       ...,                  20 , ... // font max minus min y is 20 pixels tall
//       ..., TRUETYPE_POINT_SIZE(20), ... // 'M' is 20 pixels tall

typedef struct
{
    float font_size;
    int first_unicode_codepoint_in_range; // if non-zero, then the chars are continuous, and this is the first codepoint
    int *array_of_unicode_codepoints;     // if non-zero, then this is an array of unicode codepoints
    int num_chars;
    truetype_packedchar *chardata_for_range;  // output
    unsigned char h_oversample, v_oversample; // don't set these, they're used internally
} truetype_pack_range;

int truetype_PackFontRanges(truetype_pack_context *spc, const unsigned char *fontdata, int font_index, truetype_pack_range *ranges, int num_ranges);
// Creates character bitmaps from multiple ranges of characters stored in
// ranges. This will usually create a better-packed bitmap than multiple
// calls to truetype_PackFontRange. Note that you can call this multiple
// times within a single PackBegin/PackEnd.

void truetype_PackSetOversampling(truetype_pack_context *spc, unsigned int h_oversample, unsigned int v_oversample);
// Oversampling a font increases the quality by allowing higher-quality subpixel
// positioning, and is especially valuable at smaller text sizes.
//
// This function sets the amount of oversampling for all following calls to
// truetype_PackFontRange(s) or truetype_PackFontRangesGatherRects for a given
// pack context. The default (no oversampling) is achieved by h_oversample=1
// and v_oversample=1. The total number of pixels required is
// h_oversample*v_oversample larger than the default; for example, 2x2
// oversampling requires 4x the storage of 1x1. For best results, render
// oversampled textures with bilinear filtering. Look at the readme in
// stb/tests/oversample for information about oversampled fonts
//
// To use with PackFontRangesGather etc., you must set it before calls
// call to PackFontRangesGatherRects.

void truetype_PackSetSkipMissingCodepoints(truetype_pack_context *spc, int skip);
// If skip != 0, this tells stb_truetype to skip any codepoints for which
// there is no corresponding glyph. If skip=0, which is the default, then
// codepoints without a glyph recived the font's "missing character" glyph,
// typically an empty box by convention.

int truetype_PackFontRangesGatherRects(truetype_pack_context *spc, const truetype::File *info, truetype_pack_range *ranges, int num_ranges, stbrp_rect *rects);
void truetype_PackFontRangesPackRects(truetype_pack_context *spc, stbrp_rect *rects, int num_rects);
int truetype_PackFontRangesRenderIntoRects(truetype_pack_context *spc, const truetype::File *info, truetype_pack_range *ranges, int num_ranges, stbrp_rect *rects);
// Calling these functions in sequence is roughly equivalent to calling
// truetype_PackFontRanges(). If you more control over the packing of multiple
// fonts, or if you want to pack custom data into a font texture, take a look
// at the source to of truetype_PackFontRanges() and create a custom version
// using these functions, e.g. call GatherRects multiple times,
// building up a single array of rects, then call PackRects once,
// then call RenderIntoRects repeatedly. This may result in a
// better packing than calling PackFontRanges multiple times
// (or it may not).

// this is an opaque structure that you shouldn't mess with which holds
// all the context needed from PackBegin to PackEnd.
struct truetype_pack_context
{
    void *user_allocator_context;
    void *pack_info;
    int width;
    int height;
    int stride_in_bytes;
    int padding;
    int skip_missing;
    unsigned int h_oversample, v_oversample;
    unsigned char *pixels;
    void *nodes;
};

//////////////////////////////////////////////////////////////////////////////
//
// FONT LOADING
//
//

int truetype_InitFont(truetype::File *info, const unsigned char *data);
// Given an offset into the file that defines a font, this function builds
// the necessary cached info for the rest of the system. You must allocate
// the truetype::File yourself, and truetype_InitFont will fill it out. You don't
// need to do anything special to free it, because the contents are pure
// value data with no additional data structures. Returns 0 on failure.

//////////////////////////////////////////////////////////////////////////////
//
// CHARACTER TO GLYPH-INDEX CONVERSIOn

int truetype_FindGlyphIndex(const truetype::File *info, int unicode_codepoint);
// If you're going to perform multiple operations on the same character
// and you want a speed-up, call this function with the character you're
// going to process, then use glyph-based functions instead of the
// codepoint-based functions.
// Returns 0 if the character codepoint is not defined in the font.

//////////////////////////////////////////////////////////////////////////////
//
// CHARACTER PROPERTIES
//

float truetype_ScaleForPixelHeight(const truetype::File *info, float pixels);
// computes a scale factor to produce a font whose "height" is 'pixels' tall.
// Height is measured as the distance from the highest ascender to the lowest
// descender; in other words, it's equivalent to calling truetype_GetFontVMetrics
// and computing:
//       scale = pixels / (ascent - descent)
// so if you prefer to measure height by the ascent only, use a similar calculation.

float truetype_ScaleForMappingEmToPixels(const truetype::File *info, float pixels);
// computes a scale factor to produce a font whose EM size is mapped to
// 'pixels' tall. This is probably what traditional APIs compute, but
// I'm not positive.

void truetype_GetFontVMetrics(const truetype::File *info, int *ascent, int *descent, int *lineGap);
// ascent is the coordinate above the baseline the font extends; descent
// is the coordinate below the baseline the font extends (i.e. it is typically negative)
// lineGap is the spacing between one row's descent and the next row's ascent...
// so you should advance the vertical position by "*ascent - *descent + *lineGap"
//   these are expressed in unscaled coordinates, so you must multiply by
//   the scale factor for a given size

int truetype_GetFontVMetricsOS2(const truetype::File *info, int *typoAscent, int *typoDescent, int *typoLineGap);
// analogous to GetFontVMetrics, but returns the "typographic" values from the OS/2
// table (specific to MS/Windows TTF files).
//
// Returns 1 on success (table present), 0 on failure.

void truetype_GetFontBoundingBox(const truetype::File *info, int *x0, int *y0, int *x1, int *y1);
// the bounding box around all possible characters

void truetype_GetCodepointHMetrics(const truetype::File *info, int codepoint, int *advanceWidth, int *leftSideBearing);
// leftSideBearing is the offset from the current horizontal position to the left edge of the character
// advanceWidth is the offset from the current horizontal position to the next horizontal position
//   these are expressed in unscaled coordinates

int truetype_GetCodepointKernAdvance(const truetype::File *info, int ch1, int ch2);
// an additional amount to add to the 'advance' value between ch1 and ch2

int truetype_GetCodepointBox(const truetype::File *info, int codepoint, int *x0, int *y0, int *x1, int *y1);
// Gets the bounding box of the visible part of the glyph, in unscaled coordinates

void truetype_GetGlyphHMetrics(const truetype::File *info, int glyph_index, int *advanceWidth, int *leftSideBearing);
int truetype_GetGlyphKernAdvance(const truetype::File *info, int glyph1, int glyph2);
int truetype_GetGlyphBox(const truetype::File *info, int glyph_index, int *x0, int *y0, int *x1, int *y1);
// as above, but takes one or more glyph indices for greater efficiency

typedef struct truetype_kerningentry
{
    int glyph1; // use truetype_FindGlyphIndex
    int glyph2;
    int advance;
} truetype_kerningentry;

int truetype_GetKerningTableLength(const truetype::File *info);
int truetype_GetKerningTable(const truetype::File *info, truetype_kerningentry *table, int table_length);
// Retrieves a complete list of all of the kerning pairs provided by the font
// truetype_GetKerningTable never writes more than table_length entries and returns how many entries it did write.
// The table will be sorted by (a.glyph1 == b.glyph1)?(a.glyph2 < b.glyph2):(a.glyph1 < b.glyph1)

//////////////////////////////////////////////////////////////////////////////
//
// GLYPH SHAPES (you probably don't need these, but they have to go before
// the bitmaps for C declaration-order reasons)
//

enum
{
    TRUETYPE_vmove = 1,
    TRUETYPE_vline,
    TRUETYPE_vcurve,
    TRUETYPE_vcubic
};

typedef struct
{
    short x, y, cx, cy, cx1, cy1;
    unsigned char type, padding;
} truetype_vertex;

int truetype_IsGlyphEmpty(const truetype::File *info, int glyph_index);
// returns non-zero if nothing is drawn for this glyph

int truetype_GetGlyphShape(const truetype::File *info, int glyph_index, truetype_vertex **vertices);
// returns # of vertices and fills *vertices with the pointer to them
//   these are expressed in "unscaled" coordinates
//
// The shape is a series of contours. Each one starts with
// a TRUETYPE_moveto, then consists of a series of mixed
// TRUETYPE_lineto and TRUETYPE_curveto segments. A lineto
// draws a line from previous endpoint to its x,y; a curveto
// draws a quadratic bezier from previous endpoint to
// its x,y, using cx,cy as the bezier control point.

void truetype_FreeShape(const truetype::File *info, truetype_vertex *vertices);
// frees the data allocated above

//////////////////////////////////////////////////////////////////////////////
//
// BITMAP RENDERING
//

unsigned char *truetype_GetCodepointBitmap(const truetype::File *info, float scale_x, float scale_y, int codepoint, int *width, int *height, int *xoff, int *yoff);
// allocates a large-enough single-channel 8bpp bitmap and renders the
// specified character/glyph at the specified scale into it, with
// antialiasing. 0 is no coverage (transparent), 255 is fully covered (opaque).
// *width & *height are filled out with the width & height of the bitmap,
// which is stored left-to-right, top-to-bottom.
//
// xoff/yoff are the offset it pixel space from the glyph origin to the top-left of the bitmap

unsigned char *truetype_GetCodepointBitmapSubpixel(const truetype::File *info, float scale_x, float scale_y, float shift_x, float shift_y, int codepoint, int *width, int *height, int *xoff, int *yoff);
// the same as truetype_GetCodepoitnBitmap, but you can specify a subpixel
// shift for the character

void truetype_MakeCodepointBitmap(const truetype::File *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, int codepoint);
// the same as truetype_GetCodepointBitmap, but you pass in storage for the bitmap
// in the form of 'output', with row spacing of 'out_stride' bytes. the bitmap
// is clipped to out_w/out_h bytes. Call truetype_GetCodepointBitmapBox to get the
// width and height and positioning info for it first.

void truetype_MakeCodepointBitmapSubpixel(const truetype::File *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int codepoint);
// same as truetype_MakeCodepointBitmap, but you can specify a subpixel
// shift for the character

void truetype_MakeCodepointBitmapSubpixelPrefilter(const truetype::File *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int oversample_x, int oversample_y, float *sub_x, float *sub_y, int codepoint);
// same as truetype_MakeCodepointBitmapSubpixel, but prefiltering
// is performed (see truetype_PackSetOversampling)

void truetype_GetCodepointBitmapBox(const truetype::File *font, int codepoint, float scale_x, float scale_y, int *ix0, int *iy0, int *ix1, int *iy1);
// get the bbox of the bitmap centered around the glyph origin; so the
// bitmap width is ix1-ix0, height is iy1-iy0, and location to place
// the bitmap top left is (leftSideBearing*scale,iy0).
// (Note that the bitmap uses y-increases-down, but the shape uses
// y-increases-up, so CodepointBitmapBox and CodepointBox are inverted.)

void truetype_GetCodepointBitmapBoxSubpixel(const truetype::File *font, int codepoint, float scale_x, float scale_y, float shift_x, float shift_y, int *ix0, int *iy0, int *ix1, int *iy1);
// same as truetype_GetCodepointBitmapBox, but you can specify a subpixel
// shift for the character

// the following functions are equivalent to the above functions, but operate
// on glyph indices instead of Unicode codepoints (for efficiency)
unsigned char *truetype_GetGlyphBitmap(const truetype::File *info, float scale_x, float scale_y, int glyph, int *width, int *height, int *xoff, int *yoff);
unsigned char *truetype_GetGlyphBitmapSubpixel(const truetype::File *info, float scale_x, float scale_y, float shift_x, float shift_y, int glyph, int *width, int *height, int *xoff, int *yoff);
void truetype_MakeGlyphBitmap(const truetype::File *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, int glyph);
void truetype_MakeGlyphBitmapSubpixel(const truetype::File *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int glyph);
void truetype_MakeGlyphBitmapSubpixelPrefilter(const truetype::File *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int oversample_x, int oversample_y, float *sub_x, float *sub_y, int glyph);
void truetype_GetGlyphBitmapBox(const truetype::File *font, int glyph, float scale_x, float scale_y, int *ix0, int *iy0, int *ix1, int *iy1);
void truetype_GetGlyphBitmapBoxSubpixel(const truetype::File *font, int glyph, float scale_x, float scale_y, float shift_x, float shift_y, int *ix0, int *iy0, int *ix1, int *iy1);

// @TODO: don't expose this structure
typedef struct
{
    int w, h, stride;
    unsigned char *pixels;
} truetype_bitmap;

// rasterize a shape with quadratic beziers into a bitmap
void truetype_Rasterize(truetype_bitmap *result,      // 1-channel bitmap to draw into
                        float flatness_in_pixels,     // allowable error of curve in pixels
                        truetype_vertex *vertices,    // array of vertices defining shape
                        int num_verts,                // number of vertices in above array
                        float scale_x, float scale_y, // scale applied to input vertices
                        float shift_x, float shift_y, // translation applied to input vertices
                        int x_off, int y_off,         // another translation applied to input
                        int invert);                  // if non-zero, vertically flip shape

enum
{
    // platformID
    TRUETYPE_PLATFORM_ID_UNICODE = 0,
    TRUETYPE_PLATFORM_ID_MAC = 1,
    TRUETYPE_PLATFORM_ID_ISO = 2,
    TRUETYPE_PLATFORM_ID_MICROSOFT = 3
};

enum
{
    // encodingID for TRUETYPE_PLATFORM_ID_UNICODE
    TRUETYPE_UNICODE_EID_UNICODE_1_0 = 0,
    TRUETYPE_UNICODE_EID_UNICODE_1_1 = 1,
    TRUETYPE_UNICODE_EID_ISO_10646 = 2,
    TRUETYPE_UNICODE_EID_UNICODE_2_0_BMP = 3,
    TRUETYPE_UNICODE_EID_UNICODE_2_0_FULL = 4
};

enum
{
    // encodingID for TRUETYPE_PLATFORM_ID_MICROSOFT
    TRUETYPE_MS_EID_SYMBOL = 0,
    TRUETYPE_MS_EID_UNICODE_BMP = 1,
    TRUETYPE_MS_EID_SHIFTJIS = 2,
    TRUETYPE_MS_EID_UNICODE_FULL = 10
};

enum
{
    // encodingID for TRUETYPE_PLATFORM_ID_MAC; same as Script Manager codes
    TRUETYPE_MAC_EID_ROMAN = 0,
    TRUETYPE_MAC_EID_ARABIC = 4,
    TRUETYPE_MAC_EID_JAPANESE = 1,
    TRUETYPE_MAC_EID_HEBREW = 5,
    TRUETYPE_MAC_EID_CHINESE_TRAD = 2,
    TRUETYPE_MAC_EID_GREEK = 6,
    TRUETYPE_MAC_EID_KOREAN = 3,
    TRUETYPE_MAC_EID_RUSSIAN = 7
};

enum
{
    // languageID for TRUETYPE_PLATFORM_ID_MICROSOFT; same as LCID...
    // problematic because there are e.g. 16 english LCIDs and 16 arabic LCIDs
    TRUETYPE_MS_LANG_ENGLISH = 0x0409,
    TRUETYPE_MS_LANG_ITALIAN = 0x0410,
    TRUETYPE_MS_LANG_CHINESE = 0x0804,
    TRUETYPE_MS_LANG_JAPANESE = 0x0411,
    TRUETYPE_MS_LANG_DUTCH = 0x0413,
    TRUETYPE_MS_LANG_KOREAN = 0x0412,
    TRUETYPE_MS_LANG_FRENCH = 0x040c,
    TRUETYPE_MS_LANG_RUSSIAN = 0x0419,
    TRUETYPE_MS_LANG_GERMAN = 0x0407,
    TRUETYPE_MS_LANG_SPANISH = 0x0409,
    TRUETYPE_MS_LANG_HEBREW = 0x040d,
    TRUETYPE_MS_LANG_SWEDISH = 0x041D
};

enum
{
    // languageID for TRUETYPE_PLATFORM_ID_MAC
    TRUETYPE_MAC_LANG_ENGLISH = 0,
    TRUETYPE_MAC_LANG_JAPANESE = 11,
    TRUETYPE_MAC_LANG_ARABIC = 12,
    TRUETYPE_MAC_LANG_KOREAN = 23,
    TRUETYPE_MAC_LANG_DUTCH = 4,
    TRUETYPE_MAC_LANG_RUSSIAN = 32,
    TRUETYPE_MAC_LANG_FRENCH = 1,
    TRUETYPE_MAC_LANG_SPANISH = 6,
    TRUETYPE_MAC_LANG_GERMAN = 2,
    TRUETYPE_MAC_LANG_SWEDISH = 5,
    TRUETYPE_MAC_LANG_HEBREW = 10,
    TRUETYPE_MAC_LANG_CHINESE_SIMPLIFIED = 33,
    TRUETYPE_MAC_LANG_ITALIAN = 3,
    TRUETYPE_MAC_LANG_CHINESE_TRAD = 19
};
