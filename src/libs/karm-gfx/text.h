#pragma once

#include <karm-logger/logger.h>
#include <karm-media/font.h>

#include "context.h"

namespace Karm::Gfx {

enum struct TextAlign {
    LEFT,
    CENTER,
    RIGHT,
};

struct TextStyle {
    Media::Font font;
    TextAlign align = TextAlign::LEFT;
    Opt<Color> color = NONE;
    bool wordwrap = true;
    bool multiline = false;

    TextStyle withSize(f64 size) const {
        TextStyle style = *this;
        style.font.fontsize = size;
        return style;
    }

    TextStyle withLineHeight(f64 height) const {
        TextStyle style = *this;
        style.font.lineheight = height;
        return style;
    }

    TextStyle withAlign(TextAlign align) const {
        TextStyle style = *this;
        style.align = align;
        return style;
    }

    TextStyle withColor(Color color) const {
        TextStyle style = *this;
        style.color = color;
        return style;
    }

    TextStyle withWordwrap(bool wordwrap) const {
        TextStyle style = *this;
        style.wordwrap = wordwrap;
        return style;
    }

    TextStyle withMultiline(bool multiline) const {
        TextStyle style = *this;
        style.multiline = multiline;
        return style;
    }
};

struct Text {
    struct Cell {
        Media::Glyph glyph;
        f64 pos = 0; //< Position of the glyph within the block
    };

    struct Block {
        USizeRange cells;
        usize spaces = 0;
        bool newline = false;

        f64 pos = 0; // Position of the block within the line
        f64 width = 0;

        bool empty() const {
            return cells.empty() and not spaces and not newline;
        }

        f64 fullWidth(Text const &text) const {
            return width + spaces * text._spaceWidth;
        }

        bool hasWhitespace() const {
            return spaces > 0 or newline;
        }
    };

    struct Line {
        USizeRange blocks;
        f64 baseline = 0; // Baseline of the line within the text
        f64 width = 0;
    };

    TextStyle _style;
    Vec<Cell> _cells;
    Vec<Block> _blocks;
    Vec<Line> _lines;

    // Various cached values
    bool _blocksMeasured = false;
    f64 _spaceWidth{};
    f64 _lineHeight{};

    Text(TextStyle style, Str str = "");

    // MARK: Text --------------------------------------------------------------

    void _beginBlock();

    void _appendRune(Rune rune);

    void clear();

    template <typename E>
    void append(_Str<E> str) {
        for (auto rune : iterRunes(str))
            _appendRune(rune);
    }

    void append(Slice<Rune> runes);

    // MARK: Layout -------------------------------------------------------------

    void _measureBlocks();

    void _wrapLines(f64 width);

    f64 _layoutVerticaly();

    f64 _layoutHorizontaly(f64 width);

    Math::Vec2f layout(f64 width);

    // MARK: Paint -------------------------------------------------------------

    void paint(Context &ctx) const;
};

} // namespace Karm::Gfx
