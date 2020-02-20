/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/Font.h>
#include <libsystem/Result.h>
#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/path.h>

Glyph *font_load_glyph(const char *name)
{
    char glyph_path[PATH_LENGHT];
    snprintf(glyph_path, PATH_LENGHT, "/res/font/%s.glyph", name);

    Stream *glyph_file = stream_open(glyph_path, OPEN_READ);

    if (handle_has_error(glyph_file))
    {
        logger_error("Failled to load glyph from %s: %s", glyph_path, handle_error_string(glyph_file));
        stream_close(glyph_file);

        return NULL;
    }

    FileState stat;
    stream_stat(glyph_file, &stat);
    if (stat.type != FILE_TYPE_REGULAR)
    {
        logger_error("Failled to load  glyph from %s: The glyph file isn't a regular file but %d!", glyph_path, stat.type);
        stream_close(glyph_file);

        return NULL;
    }

    Glyph *glyph = malloc(stat.size);
    size_t readed = stream_read(glyph_file, glyph, stat.size);

    if (readed != stat.size)
    {
        logger_error("Failled to load glyph from %s: %s", glyph_path, handle_error_string(glyph_file));
        stream_close(glyph_file);

        free(glyph);

        return NULL;
    }

    stream_close(glyph_file);

    return glyph;
}

Bitmap *font_load_bitmap_create(const char *name)
{
    char bitmap_path[PATH_LENGHT];
    snprintf(bitmap_path, PATH_LENGHT, "/res/font/%s.png", name);

    logger_trace("Loading fonts bitmap of %s from %s", name, bitmap_path);

    Bitmap *bitmap = bitmap_load_from(bitmap_path);

    bitmap->filtering = BITMAP_FILTERING_LINEAR;

    return bitmap;
}

Font *font_create(const char *name)
{
    logger_trace("Loading font %s...", name);

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

void font_destroy(Font *this)
{
    assert(this);

    free(this->glyph);
    bitmap_destroy(this->bitmap);
    free(this);
}

Glyph *font_glyph(Font *this, Codepoint codepoint)
{
    assert(this);

    for (int i = 0; this->glyph[i].codepoint != 0; i++)
    {
        if (this->glyph[i].codepoint == codepoint)
        {
            return &this->glyph[i];
        }
    }

    return NULL;
}

int font_measure_width(Font *this, float font_size, const char *str, int str_size)
{
    assert(this);

    int width = 0;

    for (int i = 0; i < str_size; i++)
    {
        Glyph *g = font_glyph(this, str[i]);
        width += g->advance * (font_size / 16.0);
    }

    return width;
}
