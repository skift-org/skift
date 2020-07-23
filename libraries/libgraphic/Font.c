
#include <libgraphic/Font.h>
#include <libsystem/Assert.h>
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/File.h>
#include <libsystem/io/Path.h>
#include <libsystem/Logger.h>

Glyph *font_load_glyph(const char *name)
{
    char glyph_path[PATH_LENGTH];
    snprintf(glyph_path, PATH_LENGTH, "/System/Fonts/%s.glyph", name);

    Glyph *glyph_buffer = NULL;
    size_t glyph_size = 0;
    Result result = file_read_all(glyph_path, (void **)&glyph_buffer, &glyph_size);

    if (result != SUCCESS)
    {
        logger_error("Failled to load glyph from %s: %s", glyph_path, handle_error_string(result));
        return NULL;
    }

    return glyph_buffer;
}

Bitmap *font_load_bitmap_create(const char *name)
{
    char bitmap_path[PATH_LENGTH];
    snprintf(bitmap_path, PATH_LENGTH, "/System/Fonts/%s.png", name);

    Bitmap *bitmap = bitmap_load_from(bitmap_path);

    bitmap->filtering = BITMAP_FILTERING_LINEAR;

    return bitmap;
}

Font *font_create(const char *name)
{
    Glyph *glyph = font_load_glyph(name);

    if (glyph == NULL)
    {
        logger_error("Failled to load font %s: missing glyphs", name);
        return NULL;
    }

    Bitmap *bitmap = font_load_bitmap_create(name);

    if (bitmap == NULL)
    {
        logger_error("Failled to load font %s: missing bitmap", name);

        free(glyph);
        return NULL;
    }

    Font *font = __create(Font);

    font->bitmap = bitmap;
    font->glyph = glyph;
    font->default_glyph = *font_glyph(font, '?');

    return font;
}

void font_destroy(Font *font)
{
    assert(font);

    free(font->glyph);
    bitmap_destroy(font->bitmap);
    free(font);
}

Glyph *font_glyph(Font *font, Codepoint codepoint)
{
    assert(font);

    for (int i = 0; font->glyph[i].codepoint != 0; i++)
    {
        if (font->glyph[i].codepoint == codepoint)
        {
            return &font->glyph[i];
        }
    }

    return &font->default_glyph;
}

int font_measure_width(Font *font, float font_size, const char *str, int str_size)
{
    assert(font);

    int width = 0;

    for (int i = 0; i < str_size; i++)
    {
        Glyph *g = font_glyph(font, str[i]);
        width += g->advance * (font_size / 16.0);
    }

    return width;
}

int font_measure_string(Font *font, const char *str)
{
    int width = 0;

    for (size_t i = 0; str[i]; i++)
    {
        Glyph *glyph = font_glyph(font, str[i]);
        width += glyph->advance;
    }

    return width;
}
