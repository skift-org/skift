#pragma once

#include <libgraphic/Bitmap.h>
#include <libsystem/unicode/Codepoint.h>
#include <libutils/String.h>
#include <libutils/Vector.h>

struct Glyph
{
    Codepoint codepoint;
    Rectangle bound;
    Vec2i origin;
    int advance;
};

class Font : public RefCounted<Font>
{
private:
    RefPtr<Bitmap> _bitmap;
    Glyph _default;
    Vector<Glyph> _glyphs;

public:
    Bitmap &bitmap() { return *_bitmap; }

    static ResultOr<RefPtr<Font>> create(String name);

    Font(RefPtr<Bitmap> bitmap, Vector<Glyph> glyphs)
        : _bitmap(bitmap),
          _glyphs(move(glyphs))
    {
        _default = glyph(U'?');
    }

    Glyph &glyph(Codepoint codepoint);

    bool has_glyph(Codepoint codepoint);

    Rectangle mesure_string(const char *string);
};
