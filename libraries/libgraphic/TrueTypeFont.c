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

    int bitmap_width;
    int bitmap_height;
    uint8_t *bitmap;
};

struct TrueTypeGlyph
{
    Codepoint codepoint;
    Rectangle bound;
    Vec2i offset;
    int advance;
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

    font->bitmap_width = 512;
    font->bitmap_height = 512;
    font->bitmap = (uint8_t *)malloc(font->bitmap_width * font->bitmap_height);

    stbtt_PackBegin(
        &font->rasterizer,
        font->bitmap,
        font->bitmap_width,
        font->bitmap_height,
        0,
        1,
        NULL);

    truetypefont_raster_range(font, 0x0020, 0x007f);
    truetypefont_raster_range(font, 0x00A0, 0x00FF);

    return font;
}

void truetypefont_destroy(TrueTypeFont *font)
{
    stbtt_PackEnd(&font->rasterizer);

    if (font->glyphs)
    {
        free(font->glyphs);
    }

    free(font->bitmap);
    free(font);
}

void truetypefont_raster_range(TrueTypeFont *font, Codepoint start, Codepoint end)
{
    size_t start_index = 0;
    size_t count = end - start;

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

        glyph->bound = RECTANGLE(
            packedchar[i].x0,
            packedchar[i].y0,
            packedchar[i].x1 - packedchar[i].x0,
            packedchar[i].y1 - packedchar[i].y0);
    }
}
