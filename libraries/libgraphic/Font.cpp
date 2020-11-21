#include <libgraphic/Font.h>
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/File.h>
#include <libutils/HashMap.h>
#include <libutils/Path.h>

static HashMap<String, RefPtr<Font>> _fonts;

static ResultOr<Vector<Glyph>> font_load_glyph(String name)
{
    char glyph_path[PATH_LENGTH];
    snprintf(glyph_path, PATH_LENGTH, "/Files/Fonts/%s.glyph", name.cstring());

    Glyph *glyph_buffer = nullptr;
    size_t glyph_size = 0;
    Result result = file_read_all(glyph_path, (void **)&glyph_buffer, &glyph_size);

    if (result != SUCCESS)
    {
        logger_error("Failed to load glyph from %s: %s", glyph_path, handle_error_string(result));
        return result;
    }

    return Vector(ADOPT, glyph_buffer, glyph_size / sizeof(Glyph));
}

static ResultOr<RefPtr<Bitmap>> font_load_bitmap(String name)
{
    char bitmap_path[PATH_LENGTH];
    snprintf(bitmap_path, PATH_LENGTH, "/Files/Fonts/%s.png", name.cstring());
    return Bitmap::load_from(bitmap_path);
}

ResultOr<RefPtr<Font>> Font::create(String name)
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

Glyph &Font::glyph(Codepoint codepoint)
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

bool Font::has_glyph(Codepoint codepoint)
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

Rectangle Font::mesure_string(const char *string)
{
    int width = 0;

    codepoint_foreach(reinterpret_cast<const uint8_t *>(string), [&](auto codepoint) {
        Glyph &g = glyph(codepoint);
        width += g.advance;
    });

    return Rectangle(width, 16);
}
