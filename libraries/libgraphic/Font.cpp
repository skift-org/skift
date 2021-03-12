#include <assert.h>
#include <stdio.h>

#include <libgraphic/Font.h>
#include <libio/File.h>
#include <libio/Format.h>
#include <libio/Read.h>
#include <libsystem/Logger.h>
#include <libutils/HashMap.h>
#include <libutils/Path.h>

static HashMap<String, RefPtr<Font>>
    _fonts;

static ResultOr<Vector<Glyph>> font_load_glyph(String name)
{
    auto path = IO::format("/Files/Fonts/{}.glyph", name);
    IO::File glyph_file{path, OPEN_READ};

    if (!glyph_file.exist())
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    Vector<Glyph> glyphs;

    TRY(IO::read(glyph_file, glyphs));

    return glyphs;
}

static ResultOr<RefPtr<Bitmap>> font_load_bitmap(String name)
{
    return Bitmap::load_from(IO::format("/Files/Fonts/{}.png", name));
}

ResultOr<RefPtr<Font>> Font::get(String name)
{
    if (!_fonts.has_key(name))
    {
        auto glyph_or_error = font_load_glyph(name);

        if (!glyph_or_error.success())
        {
            logger_error("Failed to load font %s: missing glyphs", name.cstring());
            return glyph_or_error.result();
        }

        auto bitmap_or_error = font_load_bitmap(name);

        if (!bitmap_or_error.success())
        {
            logger_error("Failed to load font %s: missing bitmap", name.cstring());
            return bitmap_or_error.result();
        }

        _fonts[name] = make<Font>(bitmap_or_error.take_value(), glyph_or_error.take_value());
    }

    return _fonts[name];
}

bool Font::has(Codepoint codepoint) const
{
    for (int i = 0; _glyphs[i].codepoint != 0; i++)
    {
        if (_glyphs[i].codepoint == codepoint)
        {
            return true;
        }
    }

    return false;
}

const Glyph &Font::glyph(Codepoint codepoint) const
{
    for (int i = 0; _glyphs[i].codepoint != 0; i++)
    {
        if (_glyphs[i].codepoint == codepoint)
        {
            return _glyphs[i];
        }
    }

    return _default;
}

Recti Font::mesure(Codepoint codepoint) const
{
    auto &g = glyph(codepoint);

    return {g.advance, metrics().lineheight()};
}

Recti Font::mesure(const char *string) const
{
    int width = 0;

    codepoint_foreach(reinterpret_cast<const uint8_t *>(string), [&](auto codepoint) {
        auto &g = glyph(codepoint);
        width += g.advance;
    });

    return Recti(width, metrics().lineheight());
}

Recti Font::mesure_with_fulllineheight(const char *string)
{
    int width = 0;

    codepoint_foreach(reinterpret_cast<const uint8_t *>(string), [&](auto codepoint) {
        auto &g = glyph(codepoint);
        width += g.advance;
    });

    return Recti(width, metrics().fulllineheight());
}
