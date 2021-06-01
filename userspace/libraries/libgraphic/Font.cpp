#include <stdio.h>

#include <libgraphic/Font.h>
#include <libio/File.h>
#include <libio/Format.h>
#include <libio/Path.h>
#include <libio/Read.h>
#include <libio/Streams.h>
#include <libutils/HashMap.h>

namespace Graphic
{

static HashMap<String, RefPtr<Font>>
    _fonts;

static ResultOr<Vector<Glyph>> font_load_glyph(String name)
{
    auto path = IO::format("/Files/Fonts/{}.glyph", name);
    IO::File glyph_file{path, HJ_OPEN_READ};

    if (!glyph_file.exist())
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    Vector<Glyph> glyphs;

    TRY(IO::read_vector(glyph_file, glyphs));

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
            IO::logln("Failed to load font {}: missing glyphs", name.cstring());
            return glyph_or_error.result();
        }

        auto bitmap_or_error = font_load_bitmap(name);

        if (!bitmap_or_error.success())
        {
            IO::logln("Failed to load font {}: missing bitmap", name.cstring());
            return bitmap_or_error.result();
        }

        _fonts[name] = make<Font>(bitmap_or_error.unwrap(), glyph_or_error.unwrap());
    }

    return _fonts[name];
}

bool Font::has(Text::Rune rune) const
{
    for (int i = 0; _glyphs[i].rune != 0; i++)
    {
        if (_glyphs[i].rune == rune)
        {
            return true;
        }
    }

    return false;
}

const Glyph &Font::glyph(Text::Rune rune) const
{
    for (int i = 0; _glyphs[i].rune != 0; i++)
    {
        if (_glyphs[i].rune == rune)
        {
            return _glyphs[i];
        }
    }

    return _default;
}

Math::Recti Font::mesure(Text::Rune rune) const
{
    auto &g = glyph(rune);

    return {g.advance, metrics().lineheight()};
}

Math::Recti Font::mesure(const char *string) const
{
    int width = 0;

    Text::rune_foreach(reinterpret_cast<const uint8_t *>(string), [&](auto rune) {
        auto &g = glyph(rune);
        width += g.advance;
    });

    return Math::Recti(width, metrics().lineheight());
}

Math::Recti Font::mesure_with_fulllineheight(const char *string)
{
    int width = 0;

    Text::rune_foreach(reinterpret_cast<const uint8_t *>(string), [&](auto rune) {
        auto &g = glyph(rune);
        width += g.advance;
    });

    return Math::Recti(width, metrics().fulllineheight());
}

} // namespace Graphic
