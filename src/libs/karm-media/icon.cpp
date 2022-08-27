#include <karm-gfx/context.h>

#include "icon.h"

namespace Karm::Media {

Strong<Fontface> Icon::fontface() {
    static Strong<Fontface> f = []() {
        return loadFontface("res/fonts/mdi/Material-Design-Icons.ttf").unwrap();
    }();
    return f;
}

/*
// FIXME: clang chokes on this
Result<Icon> Icon::byName(Str query, double size) {
#define ICON(id, name, code)            \
    if (Op::eq(query, Str{name}) == 0) \
        return Icon{Icons::id, size};
#include "icons.inc"
#undef ICON
    return NONE;
}
*/

/*
// FIXME: clang chokes on this
Str Icon::name() {
    switch (_code) {
#define ICON(id, name, code) \
    case Icons::id:          \
        return #name;
#include "icons.inc"
#undef ICON
    }
    return "";
}
*/

void Icon::fill(Gfx::Context &g, Math::Vec2i pos) const {
    auto face = fontface();
    auto scale = _size / face->units();

    g.save();
    g.begin();
    g.textFont({_size, face});
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
    g.textFont({_size, face});
    g.origin(pos + Math::Vec2i{0, (int)(face->metrics().ascend * scale)});
    g.scale(scale);
    face->contour(g, (Rune)_code);
    g.stroke();
    g.restore();
}

} // namespace Karm::Media
