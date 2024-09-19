#include "run.h"

namespace Karm::Text {

Run Run::from(Slice<Rune> runes) {
    Run run;
    run.append(runes);
    return run;
}

void Run::clear() {
    _runes.clear();
    _cells.clear();
    _shaped = false;
    _size = {};
}

void Run::append(Rune rune) {
    _runes.pushBack(rune);
}

void Run::append(Slice<Rune> runes) {
    _runes.ensure(_runes.len() + runes.len());
    for (auto rune : runes) {
        append(rune);
    }
}

void Run::_layout(Font &font) {
    auto xpos = 0.0f;
    bool first = true;
    Glyph prev = Glyph::TOFU;
    for (auto &cell : _cells) {
        if (not first)
            xpos += font.kern(prev, cell.glyph);
        else
            first = false;

        cell.xpos = xpos;
        cell.adv = font.advance(cell.glyph);
        xpos += cell.adv;
        prev = cell.glyph;
    }

    _size = {
        xpos,
        font.metrics().lineheight(),
    };
}

void Run::shape(Font &font) {
    _cells.clear();

    _cells.ensure(_runes.len());
    for (auto rune : _runes) {
        auto glyph = font.glyph(rune == '\n' ? ' ' : rune);
        _cells.pushBack({
            .runeRange = {_runes.len(), 1},
            .glyph = glyph,
        });
    }

    _layout(font);

    _shaped = true;
}

} // namespace Karm::Text
