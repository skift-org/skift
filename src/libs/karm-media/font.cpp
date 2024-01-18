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

    // Spread linegap evenly between lines
    m.linegap += (lineheight - 1) * (m.ascend + m.descend);

    return m;
}

Glyph Font::glyph(Rune rune) const {
    return fontface->glyph(rune);
}

f64 Font::advance(Glyph glyph) const {
    return fontface->advance(glyph) * scale();
}

f64 Font::kern(Glyph prev, Glyph curr) const {
    return fontface->kern(prev, curr) * scale();
}

FontMeasure Font::measure(Glyph r) const {
    auto m = metrics();
    auto adv = advance(r);

    return {
        .capbound = {adv, m.captop + m.descend},
        .linebound = {adv, m.ascend + m.descend + m.linegap},
        .baseline = {0, m.ascend},
    };
}

FontMeasure Font::measureStr(Str str) const {
    f64 adv = 0;

    bool first = true;
    Media::Glyph prev{0};
    for (auto r : iterRunes(str)) {
        auto curr = glyph(r);
        if (not first)
            adv += kern(prev, curr);
        else
            first = false;
        adv += advance(curr);
        prev = curr;
    }

    auto m = metrics();
    return {
        .capbound = {adv, m.captop + m.descend},
        .linebound = {adv, m.ascend + m.descend + m.linegap},
        .baseline = {0, m.ascend},
    };
}

} // namespace Karm::Media
