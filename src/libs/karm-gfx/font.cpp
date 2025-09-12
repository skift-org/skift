#include <karm-gfx/canvas.h>

#include "font.h"

namespace Karm::Gfx {

// MARK: Fallback --------------------------------------------------------------

struct VgaFontface : Fontface {
    static constexpr isize WIDTH = 8;
    static constexpr isize HEIGHT = 8;
    static constexpr f64 UNIT_PER_EM = 8;

    static constexpr Array<u8, 1024> const DATA = {
#include "defs/vga.inc"

    };

    FontMetrics metrics() override {
        return {
            .ascend = 12 / UNIT_PER_EM,
            .captop = 10 / UNIT_PER_EM,
            .descend = 4 / UNIT_PER_EM,
            .linegap = 4 / UNIT_PER_EM,
            .advance = 8 / UNIT_PER_EM,
            .xHeight = 6 / UNIT_PER_EM,
        };
    }

    FontAttrs attrs() const override {
        return {
            .family = "IBM VGA8"_sym,
            .monospace = Monospace::YES,
        };
    }

    Glyph glyph(Rune rune) override {
        One<Ibm437> one;
        encodeOne<Ibm437>(rune, one);
        return Glyph(one);
    }

    f64 advance(Glyph) override {
        return WIDTH / UNIT_PER_EM;
    }

    f64 kern(Glyph, Glyph) override {
        return 0;
    }

    void contour(Gfx::Canvas& g, Glyph glyph) const override {
        g.scale(1 / UNIT_PER_EM);
        for (isize y = 0; y < HEIGHT; y++) {
            for (isize x = 0; x < WIDTH; x++) {
                u8 byte = DATA[glyph.index * HEIGHT + y];
                if (byte & (0x80 >> x)) {
                    g.rect(Math::Recti{x, y - 8, 1, 1}.cast<f64>());
                }
            }
        }
    }
};

Rc<Fontface> Fontface::fallback() {
    return makeRc<VgaFontface>();
}

Font Font::fallback() {
    return {
        .fontface = Fontface::fallback(),
        .fontsize = 8,
    };
}

// MARK: Font Family -----------------------------------------------------------

void FontFamily::contour(Canvas& g, Glyph glyph) const {
    auto& member = _members[glyph.font];
    g.scale(_adjust.sizeAdjust * member.adjust.sizeAdjust);
    member.face->contour(g, glyph);
}

// MARK: Font ------------------------------------------------------------------

FontMetrics Font::metrics() {
    auto m = fontface->metrics();

    m.advance *= fontsize;
    m.ascend *= fontsize;
    m.captop *= fontsize;
    m.descend *= fontsize;
    m.linegap *= fontsize;

    // Spread linegap evenly between lines
    m.linegap += (lineheight - 1) * (m.ascend + m.descend);

    return m;
}

Glyph Font::glyph(Rune rune) {
    return fontface->glyph(rune);
}

f64 Font::advance(Glyph glyph) {
    return fontface->advance(glyph) * fontsize;
}

f64 Font::kern(Glyph prev, Glyph curr) {
    return fontface->kern(prev, curr) * fontsize;
}

FontMeasure Font::measure(Glyph r) {
    auto m = metrics();
    auto adv = advance(r);

    return {
        .capbound = {adv, m.captop + m.descend},
        .linebound = {adv, m.ascend + m.descend + m.linegap},
        .baseline = {0, m.ascend},
    };
}

void Font::contour(Canvas& g, Glyph glyph) {
    g.scale(fontsize);
    fontface->contour(g, glyph);
}

f64 Font::fontSize() {
    return fontsize;
}

f64 Font::xHeight() {
    // FIXME: capbound height as it is here is a font metric, not a glyph metric
    return measure(glyph('x')).capbound.height;
}

f64 Font::capHeight() {
    return measure(glyph('H')).capbound.height;
}

f64 Font::zeroAdvance() {
    return advance(glyph('0'));
}

f64 Font::lineHeight() {
    return metrics().lineheight();
}

} // namespace Karm::Gfx
