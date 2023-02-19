#include <karm-gfx/context.h>

#include "icon.h"

namespace Karm::Media {

Strong<Fontface> Icon::fontface() {
    static Strong<Fontface> f = []() {
        return loadFontface("res/fonts/mdi/Material-Design-Icons.ttf").unwrap();
    }();
    return f;
}

// NOTE: Theses functions need to be implemented in C because clang choke otherwise.

extern "C" uint32_t _Karm__Media__Icon__byName(char const *query, size_t queryLen);

extern "C" char const *_Karm__Media__Icon__name(uint32_t query);

Res<Icon> Icon::byName(Str query, double size) {
    auto codepoint = _Karm__Media__Icon__byName(query.buf(), query.len());
    if (codepoint == 0) {
        return Error::notFound("icon not found");
    }
    return Ok(Icon((Icons)codepoint, size));
}

Str Icon::name() {
    return Str(_Karm__Media__Icon__name((uint32_t)_code));
}

void Icon::fill(Gfx::Context &g, Math::Vec2i pos) const {
    auto face = fontface();
    auto scale = _size / face->units();

    g.save();
    g.begin();
    g.textFont({face, _size});
    g.origin(pos + Math::Vec2i{0, (int)(face->metrics().ascend * scale)});
    g.scale(scale);
    face->contour(g, (Rune)_code);
    g.fill();
    g.restore();
}

void Icon::stroke(Gfx::Context &g, Math::Vec2i pos) const {
    auto face = fontface();
    auto scale = _size / face->units();

    g.save();
    g.begin();
    g.textFont({face, _size});
    g.origin(pos + Math::Vec2i{0, (int)(face->metrics().ascend * scale)});
    g.scale(scale);
    face->contour(g, (Rune)_code);
    g.stroke();
    g.restore();
}

} // namespace Karm::Media
