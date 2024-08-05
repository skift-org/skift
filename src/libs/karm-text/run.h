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

    static Run from(Font font);

    template <typename E>
    static Run from(Font font, _Str<E> str) {
        auto run = from(font);
        run.append(str);
        return run;
    }

    static Run from(Font font, Slice<Rune> runes);

    void clear();

    void append(Rune rune);

    template <typename E>
    void append(_Str<E> str) {
        for (auto rune : iterRunes(str))
            append(rune);
    }

    void append(Slice<Rune> runes);

    f64 baseline();

    f64 lineheight();

    Math::Vec2f layout();

    static void _fillGlyph(Gfx::Context &g, Font const &font, Math::Vec2f baseline, Glyph glyph);

    void paint(Gfx::Context &g) const;
};

} // namespace Karm::Text
