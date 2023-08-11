#include <karm-gfx/context.h>

#include "icon.h"

namespace Karm::Media {

static Opt<Strong<Fontface>> _fontface = NONE;
Strong<Fontface> Icon::fontface() {
    if (!_fontface) {
        _fontface = Media::loadFontfaceOrFallback("bundle://mdi-font/Material-Design-Icons.ttf"_url).unwrap();
    }
    return *_fontface;
}

void Icon::fill(Gfx::Context &g, Math::Vec2i pos) const {
    auto face = fontface();
    auto scale = _size / face->units();

    g.save();
    g.begin();
    g.origin(pos + Math::Vec2i{0, (isize)(face->metrics().ascend * scale)});
    g.scale(scale);
    face->contour(g, face->glyph((Rune)_code));
    g.fill();
    g.restore();
}

void Icon::stroke(Gfx::Context &g, Math::Vec2i pos) const {
    auto face = fontface();
    auto scale = _size / face->units();

    g.save();
    g.begin();
    g.origin(pos + Math::Vec2i{0, (isize)(face->metrics().ascend * scale)});
    g.scale(scale);
    face->contour(g, face->glyph((Rune)_code));
    g.stroke();
    g.restore();
}

} // namespace Karm::Media
