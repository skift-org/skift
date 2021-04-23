#pragma once

#include <libgraphic/font/Glyph.h>
#include <libio/Reader.h>
#include <libutils/unicode/Codepoint.h>

namespace Graphic::Font
{
enum FontStyle
{
    FONT_STYLE_REGULAR,
    FONT_STYLE_BOLD,
    FONT_STYLE_ITALIC
};
class FontFace : public RefCounted<FontFace>
{
public:
    virtual String family() = 0;
    virtual FontStyle style() = 0;
    virtual Optional<Glyph> glyph(Codepoint c) = 0;
};
} // namespace Graphic::Font