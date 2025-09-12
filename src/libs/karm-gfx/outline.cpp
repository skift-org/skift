#include "outline.h"

namespace Karm::Gfx {

void Outline::paint(Gfx::Canvas& c, Math::Rectf rect, Math::Radiif radii) {
    Gfx::Borders borders;
    for (auto& s : borders.styles) {
        s = style;
    }
    for (auto& f : borders.fills) {
        f = fill;
    }
    borders.widths = Math::Insetsf(width);

    Math::Insetsf offsets(offset + width);
    if (not radii.zero())
        borders.radii = radii.grow(offsets);

    borders.paint(c, rect.grow(offsets));
}

} // namespace Karm::Gfx
