#include <libsystem/Assert.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <thirdparty/stb/stb_truetype.h>
#undef STB_TRUETYPE_IMPLEMENTATION

#include <libgraphic/Bitmap.h>
#include <libgraphic/TrueTypeFont.h>
#include <libmath/Vectors.h>
#include <libsystem/io/Stream.h>

struct TrueTypeFamily
{
    stbtt_fontinfo info;
    void *buffer;
    size_t buffer_size;
};

struct TrueTypeFont
{
    stbtt_pack_context rasterizer;

    TrueTypeFamily *family;
    int size;

    size_t glyphs_count;
    TrueTypeGlyph *glyphs;
    TrueTypeAtlas *atlas;
};

TrueTypeFamily *truetype_family_create(const char *path)
{
    Stream *font_file = stream_open(path, OPEN_READ);

    FileState state = {};
    stream_stat(font_file, &state);

    TrueTypeFamily *family = __create(TrueTypeFamily);
    family->buffer_size = state.size;
    family->buffer = malloc(state.size);

    stream_read(font_file, family->buffer, state.size);

    stbtt_InitFont(
        &family->info,
        (unsigned char *)family->buffer,
        stbtt_GetFontOffsetForIndex((unsigned char *)family->buffer, 0));

    return family;
}

void truetype_family_destroy(TrueTypeFamily *family)
{
    free(family->buffer);
    free(family);
}

TrueTypeFont *truetypefont_create(TrueTypeFamily *family, int size)
{
    TrueTypeFont *font = __create(TrueTypeFont);

    font->family = family;
    font->size = size;

    TrueTypeAtlas *atlas = (TrueTypeAtlas *)malloc(sizeof(TrueTypeAtlas) + 512 * 512);
    atlas->width = 512;
    atlas->height = 512;

    font->atlas = atlas;

    stbtt_PackBegin(
        &font->rasterizer,
        atlas->buffer,
        atlas->width,
        atlas->height,
        0,
        1,
        NULL);

    //truetypefont_raster_range(font, 0x0020, 0x007f);
    //truetypefont_raster_range(font, 0x00A0, 0x00FF);

    return font;
}

void truetypefont_destroy(TrueTypeFont *font)
{
    stbtt_PackEnd(&font->rasterizer);

    if (font->glyphs)
    {
        free(font->glyphs);
    }

    free(font->atlas);
    free(font);
}

void truetypefont_raster_range(TrueTypeFont *font, Codepoint start, Codepoint end)
{
    size_t start_index = 0;
    size_t count = end - start + 1;

    if (!font->glyphs)
    {
        font->glyphs_count = count;
        font->glyphs = (TrueTypeGlyph *)calloc(font->glyphs_count, sizeof(TrueTypeGlyph));
    }
    else
    {
        start_index = font->glyphs_count;

        font->glyphs_count += count;
        font->glyphs = (TrueTypeGlyph *)realloc(font->glyphs, font->glyphs_count * sizeof(TrueTypeGlyph));
    }

    __cleanup_malloc stbtt_packedchar *packedchar = (stbtt_packedchar *)calloc(0x02AF, sizeof(stbtt_packedchar));
    stbtt_PackFontRange(&font->rasterizer, (unsigned char *)font->family->buffer, 0, font->size, start, count, packedchar);

    for (size_t i = 0; i < count; i++)
    {
        TrueTypeGlyph *glyph = &font->glyphs[start_index + i];

        glyph->advance = packedchar[i].xadvance;

        glyph->offset = vec2i(packedchar[i].xoff, packedchar[i].yoff);

        glyph->codepoint = start + i;

        glyph->bound = RECTANGLE(
            packedchar[i].x0,
            packedchar[i].y0,
            packedchar[i].x1 - packedchar[i].x0,
            packedchar[i].y1 - packedchar[i].y0);
    }
}

TrueTypeAtlas *truetypefont_get_atlas(TrueTypeFont *font)
{
    return font->atlas;
}

static TrueTypeGlyph *lookup_glyph(TrueTypeFont *font, Codepoint codepoint)
{
    for (size_t i = 0; i < font->glyphs_count; i++)
    {
        if (font->glyphs[i].codepoint == codepoint)
        {
            return &font->glyphs[i];
        }
    }

    return NULL;
}

TrueTypeGlyph *truetypefont_get_glyph_for_codepoint(TrueTypeFont *font, Codepoint codepoint)
{
    TrueTypeGlyph *glyph = lookup_glyph(font, codepoint);

    if (glyph != NULL)
    {
        return glyph;
    }

    truetypefont_raster_range(font, codepoint, codepoint);

    return lookup_glyph(font, codepoint);
}

Rectangle truetypefont_mesure_string(TrueTypeFont *font, const char *string)
{
    Codepoint codepoint = 0;

    int width = 0;

    size_t size = utf8_to_codepoint((const uint8_t *)string, &codepoint);
    string += size;
    while (size && codepoint != 0)
    {
        TrueTypeGlyph *glyph = truetypefont_get_glyph_for_codepoint(font, codepoint);

        width += glyph->advance;

        size_t size = utf8_to_codepoint((const uint8_t *)string, &codepoint);
        string += size;
    }

    return RECTANGLE(0, 0, width, font->size);
}
