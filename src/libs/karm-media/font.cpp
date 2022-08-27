#include <karm-gfx/context.h>

#include "font-vga.h"
#include "font.h"

namespace Karm::Media {

Font Font::fallback() {
    return {8, makeStrong<VgaFont>()};
}

FontMetrics Font::metrics() const {
    auto m = _face->metrics();

    m.advance *= _size / _face->units();
    m.ascend *= _size / _face->units();
    m.captop *= _size / _face->units();
    m.descend *= _size / _face->units();
    m.linegap *= _size / _face->units();

    return m;
}

double Font::advance(Rune c) const {
    return _face->advance(c) * (_size / _face->units());
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
    double adv = 0;
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

void Font::fillRune(Gfx::Context &g, Math::Vec2f baseline, Rune rune) const {
    g.save();
    g.begin();
    g.origin(baseline.cast<int>());
    g.scale(_size / _face->units());
    _face->contour(g, rune);
    g.fill();
    g.restore();
}

void Font::strokeRune(Gfx::Context &g, Math::Vec2f baseline, Rune rune) const {
    g.save();
    g.begin();
    g.origin(baseline.cast<int>());
    g.scale(_size / _face->units());
    _face->contour(g, rune);
    g.stroke();
    g.restore();
}

void Font::fillStr(Gfx::Context &g, Math::Vec2f baseline, Str str) const {
    for (auto r : iterRunes(str)) {
        fillRune(g, baseline, r);
        baseline.x += advance(r);
    }
}

void Font::strokeStr(Gfx::Context &g, Math::Vec2f baseline, Str str) const {
    for (auto r : iterRunes(str)) {
        strokeRune(g, baseline, r);
        baseline.x += advance(r);
    }
}

} // namespace Karm::Media
