#include "run.h"

namespace Karm::Text {

Run Run::from(Font font) {
    return {font};
}

Run Run::from(Font font, Slice<Rune> runes) {
    auto run = from(font);
    run.append(runes);
    return run;
}

void Run::clear() {
    _runes.clear();
    _cells.clear();
}

void Run::append(Rune rune) {
    auto glyph = _font.glyph(rune == '\n' ? ' ' : rune);
    _cells.pushBack({
        .runeRange = {_runes.len(), 1},
        .glyph = glyph,
    });
    _runes.pushBack(rune);
}

void Run::append(Slice<Rune> runes) {
    _runes.ensure(_runes.len() + runes.len());
    for (auto rune : runes) {
        append(rune);
    }
}

f64 Run::baseline() {
    auto m = _font.metrics();
    return m.linegap / 2 + m.ascend;
}

f64 Run::lineheight() {
    auto m = _font.metrics();
    return m.linegap + m.ascend + m.descend;
}

Math::Vec2f Run::layout() {
    auto xpos = 0.0f;
    bool first = true;
    Glyph prev = Glyph::TOFU;
    for (auto &cell : _cells) {
        if (not first)
            xpos += _font.kern(prev, cell.glyph);
        else
            first = false;

        cell.xpos = xpos;
        cell.adv = _font.advance(cell.glyph);
        xpos += cell.adv;
        prev = cell.glyph;
    }
    _width = xpos;

    return {
        _width,
        lineheight(),
    };
}

} // namespace Karm::Text
