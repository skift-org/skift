#pragma once

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

    Vec<Rune> _runes{};
    Vec<Cell> _cells{};
    Math::Vec2f _size{};
    bool _shaped = false;

    // MARK: Access ------------------------------------------------------------

    Slice<Rune> runes() const {
        return _runes;
    }

    Slice<Cell> cells() const {
        if (not _shaped)
            panic("run not shaped");
        return _cells;
    }

    Math::Vec2f size() const {
        if (not _shaped)
            panic("run not shaped");
        return _size;
    }

    bool shaped() const {
        return _shaped;
    }

    // MARK: Builder -----------------------------------------------------------

    template <typename E>
    static Run from(_Str<E> str) {
        Run run;
        run.append(str);
        return run;
    }

    static Run from(Slice<Rune> runes);

    void clear();

    void append(Rune rune);

    template <typename E>
    void append(_Str<E> str) {
        for (auto rune : iterRunes(str))
            append(rune);
    }

    void append(Slice<Rune> runes);

    // MARK: Layout ------------------------------------------------------------

    void _layout(Font &font);

    void shape(Font &font);
};

} // namespace Karm::Text
