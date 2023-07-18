#include <karm-gfx/context.h>

#include "font-vga.h"
#include "font.h"

namespace Karm::Media {

Strong<Fontface> Fontface::fallback() {
    return makeStrong<VgaFontface>();
}

Font Font::fallback() {
    return {
        .fontface = Fontface::fallback(),
        .fontsize = 8,
    };
}

f64 Font::scale() const {
    return fontsize / fontface->units();
}

FontMetrics Font::metrics() const {
    auto m = fontface->metrics();

    m.advance *= scale();
    m.ascend *= scale();
    m.captop *= scale();
    m.descend *= scale();
    m.linegap *= scale();

    return m;
}

f64 Font::advance(Rune c) const {
    return fontface->advance(c) * scale();
}

f64 Font::kern(Rune prev, Rune curr) const {
    return fontface->kern(prev, curr) * scale();
}

FontMesure Font::mesureRune(Rune r) const {
    auto m = metrics();
    auto adv = advance(r);

    return {
        .capbound = {adv, m.captop + m.descend},
        .linebound = {adv, m.ascend + m.descend},
        .baseline = {0, m.ascend},
    };
}

FontMesure Font::mesureStr(Str str) const {
    f64 adv = 0;
    for (auto r : iterRunes(str)) {
        adv += advance(r);
    }

    auto m = metrics();
    return {
        .capbound = {adv, m.captop + m.descend},
        .linebound = {adv, m.ascend + m.descend},
        .baseline = {0, m.ascend},
    };
}

} // namespace Karm::Media
