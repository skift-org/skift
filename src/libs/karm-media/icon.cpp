#include <karm-gfx/context.h>

#include "icon.h"

namespace Karm::Media {

static Opt<Strong<Text::Fontface>> _fontface = NONE;

Strong<Text::Fontface> Icon::fontface() {
    if (not _fontface)
        _fontface = Text::loadFontfaceOrFallback("bundle://fonts-mdi/fonts/Material-Design-Icons.ttf"_url).unwrap();
    return *_fontface;
}

void Icon::fill(Gfx::Context &g, Math::Vec2i pos) const {
    auto face = fontface();
    g.save();
    g.beginPath();
    g.origin(pos + Math::Vec2i{0, (isize)(face->metrics().ascend * _size)});
    g.scale(_size);
    face->contour(g, face->glyph((Rune)_code));
    g.fill();
    g.restore();
}

void Icon::stroke(Gfx::Context &g, Math::Vec2i pos) const {
    auto face = fontface();

    g.save();
    g.beginPath();
    g.origin(pos + Math::Vec2i{0, (isize)(face->metrics().ascend * _size)});
    g.scale(_size);
    face->contour(g, face->glyph((Rune)_code));
    g.stroke();
    g.restore();
}

} // namespace Karm::Media
