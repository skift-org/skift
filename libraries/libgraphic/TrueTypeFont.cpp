#include <libgraphic/Bitmap.h>
#include <libgraphic/TrueType.h>
#include <libgraphic/TrueTypeFont.h>
#include <libsystem/io/Stream.h>
#include <libsystem/math/Vectors.h>

struct TrueTypeFamily
{
    truetype::File info;
    void *buffer;
    size_t buffer_size;
};

struct TrueTypeFont
{
    truetype_pack_context rasterizer;

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

    truetype_InitFont(&family->info, (unsigned char *)family->buffer);

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

    truetype_PackBegin(
        &font->rasterizer,
        atlas->buffer,
        atlas->width,
        atlas->height,
        0,
        1);

    //truetypefont_raster_range(font, 0x0020, 0x007f);
    //truetypefont_raster_range(font, 0x00A0, 0x00FF);

    return font;
}

void truetypefont_destroy(TrueTypeFont *font)
{
    truetype_PackEnd(&font->rasterizer);

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

    __cleanup_malloc truetype_packedchar *packedchar = (truetype_packedchar *)calloc(0x02AF, sizeof(truetype_packedchar));
    truetype_PackFontRange(&font->rasterizer, (unsigned char *)font->family->buffer, font->size, start, count, packedchar);

    for (size_t i = 0; i < count; i++)
    {
        TrueTypeGlyph *glyph = &font->glyphs[start_index + i];

        glyph->advance = packedchar[i].xadvance;

        glyph->offset = Vec2i(packedchar[i].xoff, packedchar[i].yoff);

        glyph->codepoint = start + i;

        glyph->bound = Rectangle(
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

    return nullptr;
}

TrueTypeGlyph *truetypefont_get_glyph_for_codepoint(TrueTypeFont *font, Codepoint codepoint)
{
    TrueTypeGlyph *glyph = lookup_glyph(font, codepoint);

    if (glyph != nullptr)
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

    return Rectangle(width, font->size);
}

#include <libsystem/Logger.h>

int truetypefont_get_kerning_for_codepoints(TrueTypeFont *font, Codepoint left, Codepoint right)
{
    auto scale = truetype_ScaleForPixelHeight(&font->family->info, font->size);

    int kerning = truetype_GetCodepointKernAdvance(&font->family->info, left, right);

    return kerning * scale;
}

TrueTypeFontMetrics truetypefont_get_metrics(TrueTypeFont *font)
{
    TrueTypeFontMetrics metrics = {};

    auto scale = truetype_ScaleForPixelHeight(&font->family->info, font->size);

    truetype_GetFontVMetrics(
        &font->family->info,

        &metrics.ascent,
        &metrics.descent,
        &metrics.linegap);

    metrics.ascent *= scale;
    metrics.descent *= scale;
    metrics.linegap *= scale;

    return metrics;
}
