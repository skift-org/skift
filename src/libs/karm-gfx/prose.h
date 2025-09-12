#pragma once

#include <karm-math/au.h>

#include "color.h"
#include "font.h"

namespace Karm::Gfx {

enum struct TextAlign {
    LEFT,
    CENTER,
    RIGHT,
};

struct ProseStyle {
    Font font;
    TextAlign align = TextAlign::LEFT;
    Opt<Color> color = NONE;
    bool wordwrap = true;
    bool multiline = false;

    ProseStyle withSize(f64 size) const {
        ProseStyle style = *this;
        style.font.fontsize = size;
        return style;
    }

    ProseStyle withLineHeight(f64 height) const {
        ProseStyle style = *this;
        style.font.lineheight = height;
        return style;
    }

    ProseStyle withAlign(TextAlign align) const {
        ProseStyle style = *this;
        style.align = align;
        return style;
    }

    ProseStyle withColor(Color color) const {
        ProseStyle style = *this;
        style.color = color;
        return style;
    }

    ProseStyle withWordwrap(bool wordwrap) const {
        ProseStyle style = *this;
        style.wordwrap = wordwrap;
        return style;
    }

    ProseStyle withMultiline(bool multiline) const {
        ProseStyle style = *this;
        style.multiline = multiline;
        return style;
    }
};

struct Prose : Meta::Pinned {
    struct Span {
        Opt<Rc<Span>> parent;
        Opt<Color> color;
    };

    struct StrutCell {
        usize id;
        Vec2Au size{};
        // NOTE: baseline is distance from strut's top to the considered baseline
        Au baseline{};

        void repr(Io::Emit& e) const {
            e("(StrutCell id: {} size: {} baseline: {})", id, size, baseline);
        }
    };

    struct Cell {
        MutCursor<Prose> prose;
        Opt<Rc<Span>> span;

        urange runeRange;
        Glyph glyph;
        Au pos = 0_au; //< Position of the glyph within the block
        Au adv = 0_au; //< Advance of the glyph

        Opt<usize> relatedStrutIndex = NONE;

        void measureAdvance() {
            if (strut()) {
                adv = prose->_struts[relatedStrutIndex.unwrap()].size.x;
            } else {
                adv = Au{prose->_style.font.advance(glyph)};
            }
        }

        MutSlice<Rune> runes() {
            return mutSub(prose->_runes, runeRange);
        }

        Slice<Rune> runes() const {
            return sub(prose->_runes, runeRange);
        }

        bool newline() const {
            auto r = runes();
            if (not r)
                return false;
            return last(r) == '\n';
        }

        bool space() const {
            auto r = runes();
            if (not r)
                return false;
            return last(r) == '\n' or isAsciiSpace(last(r));
        }

        Cursor<StrutCell> strut() const {
            if (relatedStrutIndex == NONE)
                return nullptr;

            return &prose->_struts[relatedStrutIndex.unwrap()];
        }

        MutCursor<StrutCell> strut() {
            if (relatedStrutIndex == NONE)
                return nullptr;

            return &prose->_struts[relatedStrutIndex.unwrap()];
        }

        Au Yposition(Au dominantBaselineYPosition) const {
            if (relatedStrutIndex == NONE)
                return dominantBaselineYPosition;

            return dominantBaselineYPosition - prose->_struts[relatedStrutIndex.unwrap()].baseline;
        }
    };

    struct Block {
        MutCursor<Prose> prose;

        urange runeRange;
        urange cellRange;

        Au pos = 0_au; // Position of the block within the line
        Au width = 0_au;

        MutSlice<Cell> cells() {
            return mutSub(prose->_cells, cellRange);
        }

        Slice<Cell> cells() const {
            return sub(prose->_cells, cellRange);
        }

        bool empty() const {
            return cellRange.empty();
        }

        bool spaces() const {
            if (empty())
                return false;
            return last(cells()).space();
        }

        bool newline() const {
            if (empty())
                return false;
            return last(cells()).newline();
        }

        MutCursor<StrutCell> strut() {
            if (empty())
                return nullptr;

            auto cellsRef = cells();
            return last(cellsRef).strut();
        }

        Cursor<StrutCell> strut() const {
            if (empty())
                return nullptr;

            return last(cells()).strut();
        }
    };

    struct Line {
        MutCursor<Prose> prose;

        urange runeRange;
        urange blockRange;
        Au baseline = 0_au; // Baseline of the line within the text
        Au width = 0_au;

        Slice<Block> blocks() const {
            return sub(prose->_blocks, blockRange);
        }

        MutSlice<Block> blocks() {
            return mutSub(prose->_blocks, blockRange);
        }
    };

    ProseStyle _style;

    Vec<Rune> _runes;
    Vec<Cell> _cells;
    Vec<Block> _blocks;
    Vec<Line> _lines;

    Vec<StrutCell> _struts;
    Vec<usize> _strutCellsIndexes;

    // Various cached values
    bool _blocksMeasured = false;
    f64 _spaceWidth{};
    f64 _lineHeight{};

    Vec2Au _size;

    Prose(ProseStyle style, Str str = "");

    Vec2Au size() const {
        return _size;
    }

    // MARK: Prose --------------------------------------------------------------

    void _beginBlock();

    void append(Rune rune);

    void clear();

    template <typename E>
    void append(_Str<E> str) {
        for (auto rune : iterRunes(str))
            append(rune);
    }

    void append(Slice<Rune> runes);
    void append(StrutCell&& strut);

    // MARK: Span --------------------------------------------------------------

    Vec<Rc<Span>> _spans;
    Opt<Rc<Span>> _currentSpan = NONE;

    void pushSpan() {
        if (_currentSpan == NONE)
            _spans.pushBack(makeRc<Span>(Span{}));
        else
            _spans.pushBack(makeRc<Span>(_currentSpan->unwrap()));

        last(_spans)->parent = _currentSpan;

        auto refToLast = last(_spans);
        _currentSpan = refToLast;
    }

    void spanColor(Color color) {
        if (not _currentSpan)
            return;

        _currentSpan.unwrap()->color = color;
    }

    void popSpan() {
        if (not _currentSpan)
            return;

        auto newCurr = _currentSpan.unwrap()->parent;
        _currentSpan = newCurr;
    }

    void overrideSpanStackWith(Prose const& prose);

    // MARK: Strut ------------------------------------------------------------

    Vec<MutCursor<Cell>> cellsWithStruts() {
        // FIXME: Vec of MutCursor of length 1 is bad design, try to use Generator
        Vec<MutCursor<Cell>> cells;
        for (auto i : _strutCellsIndexes)
            cells.pushBack(&_cells[i]);
        return cells;
    }

    // MARK: Layout ------------------------------------------------------------

    void _measureBlocks();

    void _wrapLines(Au width);

    Au _layoutVerticaly();

    Au _layoutHorizontaly(Au width);

    Vec2Au layout(Au width);

    // MARK: Paint -------------------------------------------------------------

    void paintCaret(Canvas& g, usize runeIndex, Color color);

    void paintSelection(Canvas& g, usize start, usize end, Color color);

    struct Lbc {
        usize li, bi, ci;
    };

    Lbc lbcAt(usize runeIndex) const {
        auto maybeLi = searchLowerBound(
            _lines, [&](Line const& l) {
                return l.runeRange.start <=> runeIndex;
            }
        );

        if (not maybeLi)
            return {0, 0, 0};

        auto li = *maybeLi;

        auto& line = _lines[li];

        auto maybeBi = searchLowerBound(
            line.blocks(), [&](Block const& b) {
                return b.runeRange.start <=> runeIndex;
            }
        );

        if (not maybeBi)
            return {li, 0, 0};

        auto bi = *maybeBi;

        auto& block = line.blocks()[bi];

        auto maybeCi = searchLowerBound(
            block.cells(), [&](Cell const& c) {
                return c.runeRange.start <=> runeIndex;
            }
        );

        if (not maybeCi)
            return {li, bi, 0};

        auto ci = *maybeCi;

        auto cell = block.cells()[ci];

        if (cell.runeRange.end() == runeIndex) {
            // Handle the case where the rune is the last of the text
            ci++;
        }

        return {li, bi, ci};
    }

    Vec2Au queryPosition(usize runeIndex) const {
        auto [li, bi, ci] = lbcAt(runeIndex);

        if (isEmpty(_lines))
            return {};

        auto& line = _lines[li];

        if (isEmpty(line.blocks()))
            return {0_au, line.baseline};

        auto& block = line.blocks()[bi];

        if (isEmpty(block.cells()))
            return {block.pos, line.baseline};

        if (ci >= block.cells().len()) {
            // Handle the case where the rune is the last of the text
            auto& cell = last(block.cells());
            return {block.pos + cell.pos + cell.adv, cell.Yposition(line.baseline)};
        }

        auto& cell = block.cells()[ci];

        return {block.pos + cell.pos, cell.Yposition(line.baseline)};
    }
};

} // namespace Karm::Gfx
