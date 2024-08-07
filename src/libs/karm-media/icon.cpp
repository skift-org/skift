#include <karm-gfx/canvas.h>

#include "icon.h"

namespace Karm::Media {

static Opt<Strong<Text::Fontface>> _fontface = NONE;

Strong<Text::Fontface> Icon::fontface() {
    if (not _fontface)
        _fontface = Text::loadFontfaceOrFallback("bundle://fonts-mdi/fonts/Material-Design-Icons.ttf"_url).unwrap();
    return *_fontface;
}

void Icon::fill(Gfx::Canvas &g, Math::Vec2i pos) const {
    auto face = fontface();
    g.push();
    g.beginPath();
    g.origin(pos + Math::Vec2f{0, face->metrics().ascend * _size});
    g.scale(_size);
    face->contour(g, face->glyph((Rune)_code));
    g.fill();
    g.pop();
}

void Icon::stroke(Gfx::Canvas &g, Math::Vec2i pos) const {
    auto face = fontface();

    g.push();
    g.beginPath();
    g.origin(pos + Math::Vec2f{0, face->metrics().ascend * _size});
    g.scale(_size);
    face->contour(g, face->glyph((Rune)_code));
    g.stroke();
    g.pop();
}

} // namespace Karm::Media
