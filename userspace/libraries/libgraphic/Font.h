#pragma once

#include <libgraphic/Bitmap.h>
#include <libtext/Rune.h>
#include <libutils/String.h>
#include <libutils/Vector.h>

namespace Graphic
{

struct FontMetrics
{
private:
    int _lineheight;
    int _baseline;
    int _capheight;
    int _leading;

public:
    int lineheight() const { return _lineheight; }
    int baseline() const { return _baseline; }
    int capheight() const { return _capheight; }
    int leading() const { return _leading; }

    int fulllineheight() const { return _lineheight + _leading; }
    int halfleading() const { return leading() / 2; }

    int captop(int baseline) const { return baseline - _capheight; }
    int ascend(int baseline) const { return baseline - _baseline; }
    int descend(int baseline) const { return ascend(baseline) + _lineheight; }

    int fullascend(int baseline) const { return ascend(baseline) - halfleading(); }
    int fulldescend(int baseline) const { return descend(baseline) + halfleading(); }

    FontMetrics(int lineheight, int baseline, int capheight, int leading)
        : _lineheight(lineheight),
          _baseline(baseline),
          _capheight(capheight),
          _leading(leading)
    {
    }
};

struct Glyph
{
    Text::Rune rune;
    Math::Recti bound;
    Math::Vec2i origin;
    int advance;
};

struct Font : public RefCounted<Font>
{
private:
    RefPtr<Bitmap> _bitmap;
    Glyph _default;
    Vector<Glyph> _glyphs;

public:
    const FontMetrics metrics() const
    {
        return {16, 12, 10, 4};
    }

    Bitmap &bitmap() { return *_bitmap; }

    static ResultOr<RefPtr<Font>> get(String name);

    Font(RefPtr<Bitmap> bitmap, Vector<Glyph> glyphs)
        : _bitmap(bitmap),
          _glyphs(std::move(glyphs))
    {
        _default = glyph(U'?');
    }

    bool has(Text::Rune rune) const;

    const Glyph &glyph(Text::Rune rune) const;

    Math::Recti mesure(Text::Rune rune) const;

    Math::Recti mesure(const char *string) const;

    Math::Recti mesure_with_fulllineheight(const char *string);
};

} // namespace Graphic
