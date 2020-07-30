#pragma once

#include <libgraphic/Bitmap.h>
#include <libsystem/unicode/Codepoint.h>
#include <libsystem/utils/String.h>
#include <libsystem/utils/Vector.h>

typedef struct
{
    Codepoint codepoint;
    Rectangle bound;
    Vec2i origin;
    int advance;
} Glyph;

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
          _glyphs(glyphs)
    {
        _default = glyph(U'?');
    }

    Glyph &glyph(Codepoint codepoint);

    bool has_glyph(Codepoint codepoint);

    Rectangle mesure_string(const char *string);
};
