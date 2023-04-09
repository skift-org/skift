#include <karm-gfx/context.h>

#include "icon.h"

namespace Karm::Media {

Strong<Fontface> Icon::fontface() {
#ifdef __osdk_sys_efi__
    // NOTE: This is a workaround for a bug in ms-abi where static variables
    //       are not initialized properly.
    return loadFontface("res/fonts/mdi/Material-Design-Icons.ttf").unwrap();
#else
    static Strong<Fontface> f = []() {
        return loadFontface("res/fonts/mdi/Material-Design-Icons.ttf").unwrap();
    }();
    return f;
#endif
}

void Icon::fill(Gfx::Context &g, Math::Vec2i pos) const {
    auto face = fontface();
    auto scale = _size / face->units();

    g.save();
    g.begin();
    g.textFont({face, _size});
    g.origin(pos + Math::Vec2i{0, (isize)(face->metrics().ascend * scale)});
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
    g.origin(pos + Math::Vec2i{0, (isize)(face->metrics().ascend * scale)});
    g.scale(scale);
    face->contour(g, (Rune)_code);
    g.stroke();
    g.restore();
}

} // namespace Karm::Media
