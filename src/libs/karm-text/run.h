#pragma once

#include <karm-gfx/context.h>

#include "base.h"
#include "font.h"

namespace Karm::Text {

struct Run {
    struct Cell {
        urange runeRange;
        Glyph glyph;
        f64 xpos = 0; //< Position of the glyph within the block
        f64 adv = 0;  //< Advance of the glyph

        MutSlice<Rune> runes(Run &t) {
            return mutSub(t._runes, runeRange);
        }

        Slice<Rune> runes(Run const &t) const {
            return sub(t._runes, runeRange);
        }

        bool newline(Run const &t) const {
            auto r = runes(t);
            if (not r)
                return false;
            return last(r) == '\n';
        }

        bool space(Run const &t) const {
            auto r = runes(t);
            if (not r)
                return false;
            return last(r) == '\n' or isAsciiSpace(last(r));
        }
    };

    Font _font;
    Vec<Rune> _runes{};
    Vec<Cell> _cells{};
    f64 _width = 0;

    static Run from(Font font) {
        return {font};
    }

    template <typename E>
    static Run from(Font font, _Str<E> str) {
        auto run = from(font);
        run.append(str);
        return run;
    }

    static Run from(Font font, Slice<Rune> runes) {
        auto run = from(font);
        run.append(runes);
        return run;
    }

    void clear() {
        _runes.clear();
        _cells.clear();
    }

    void append(Rune rune) {
        auto glyph = _font.glyph(rune == '\n' ? ' ' : rune);
        _cells.pushBack({
            .runeRange = {_runes.len(), 1},
            .glyph = glyph,
        });
        _runes.pushBack(rune);
    }

    template <typename E>
    void append(_Str<E> str) {
        for (auto rune : iterRunes(str))
            append(rune);
    }

    void append(Slice<Rune> runes) {
        _runes.ensure(_runes.len() + runes.len());
        for (auto rune : runes) {
            append(rune);
        }
    }

    f64 baseline() {
        auto m = _font.metrics();
        return m.linegap / 2 + m.ascend;
    }

    f64 lineheight() {
        auto m = _font.metrics();
        return m.linegap + m.ascend + m.descend;
    }

    Math::Vec2f layout() {
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
};

} // namespace Karm::Text
