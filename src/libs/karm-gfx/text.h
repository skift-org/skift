#pragma once

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
    TextStyle _style;

    struct Cell {
        Media::Glyph glyph;
        f64 pos = 0; // Position of the glyph within the block
    };

    Vec<Cell> _cells;

    struct Block {
        USizeRange cells;
        size_t spaces = 0;
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

    Vec<Block> _blocks;
    bool _blocksMesured = false;

    struct Line {
        USizeRange blocks;
        f64 baseline = 0; // Baseline of the line within the text
        f64 width = 0;
    };

    Vec<Line> _lines;

    f64 _spaceWidth{};

    Text(TextStyle style, Str str = "") : _style(style) {
        clear();
        _spaceWidth = _style.font.advance(_style.font.glyph(' '));
        append(str);
    }

    void _beginBlock() {
        _blocks.pushBack({{_cells.len(), 0}});
    }

    void _appendRune(Rune rune) {
        if (last(_blocks).newline)
            _beginBlock();

        if (rune == ' ') {
            last(_blocks).spaces++;
        } else if (rune == '\t') {
            // HACK: tab width is 4 spaces
            last(_blocks).spaces += 4;
        } else if (rune == '\n') {
            last(_blocks).newline = true;
        } else {
            if (last(_blocks).hasWhitespace())
                _beginBlock();

            auto glyph = _style.font.glyph(rune);
            _cells.pushBack({glyph});
            last(_blocks).cells.size++;
        }
    }

    void clear() {
        _cells.clear();
        _blocks.clear();
        _blocksMesured = false;
        _blocks.pushBack({{0, 0}});
        _lines.clear();
    }

    template <typename E>
    void append(_Str<E> str) {
        for (auto rune : iterRunes(str)) {
            _appendRune(rune);
        }
    }

    void _mesureBlocks() {
        for (auto &block : _blocks) {
            auto adv = 0;
            bool first = true;
            Media::Glyph prev{0};
            for (usize i = block.cells.start; i < block.cells.end(); i++) {
                auto &cell = _cells[i];
                if (not first)
                    adv += _style.font.kern(prev, cell.glyph);
                else
                    first = false;

                cell.pos = adv;
                adv += _style.font.advance(cell.glyph);
                prev = cell.glyph;
            }
            block.width = adv;
        }
    }

    void _wrapLines(f64 width) {
        _lines.clear();

        Line line{{0, 0}};
        bool first = true;
        f64 adv = 0;
        for (usize i = 0; i < _blocks.len(); i++) {
            auto &block = _blocks[i];
            auto fullWidth = block.fullWidth(_style);
            if (adv + block.width > width and _style.wordwrap and _style.multiline and not first) {
                _lines.pushBack(line);
                line = {{i, 1}};
                adv = fullWidth;

                if (block.newline) {
                    _lines.pushBack(line);
                    line = {{i + 1, 0}};
                    adv = 0;
                }
            } else {
                line.blocks.size++;

                if (block.newline and _style.multiline) {
                    _lines.pushBack(line);
                    line = {{i + 1, 0}};
                    adv = 0;
                } else {
                    adv += fullWidth;
                }
            }
            first = false;
        }

        _lines.pushBack(line);
    }

    f64 _layoutVerticaly() {
        f64 baseline = 0;
        auto m = _style.font.metrics();
        for (auto &line : _lines) {
            baseline += m.ascend;
            line.baseline = baseline;
            baseline += m.linegap + m.descend;
        }
        return baseline;
    }

    f64 _layoutHorizontaly(f64 width) {
        f64 maxWidth = 0;
        for (auto &line : _lines) {
            if (not line.blocks.any())
                continue;

            f64 pos = 0;
            for (usize i = line.blocks.start; i < line.blocks.end(); i++) {
                auto &block = _blocks[i];
                block.pos = pos;
                pos += block.fullWidth(*this);
            }

            auto lastBlock = _blocks[line.blocks.end() - 1];
            line.width = lastBlock.pos + lastBlock.width;
            maxWidth = max(maxWidth, line.width);
            auto free = width - line.width;

            switch (_style.align) {
            case TextAlign::LEFT:
                break;

            case TextAlign::CENTER:
                for (usize i = line.blocks.start; i < line.blocks.end(); i++) {
                    auto &block = _blocks[i];
                    block.pos += free / 2;
                }
                break;

            case TextAlign::RIGHT:
                for (usize i = line.blocks.start; i < line.blocks.end(); i++) {
                    auto &block = _blocks[i];
                    block.pos += free;
                }
                break;
            }
        }

        return maxWidth;
    }

    Math::Vec2f layout(f64 width) {
        if (isEmpty(_blocks)) {
            return {};
        }

        if (not _blocksMesured) {
            _mesureBlocks();
            _blocksMesured = true;
        }

        _wrapLines(width);
        auto textHeight = _layoutVerticaly();
        auto textWidth = _layoutHorizontaly(width);
        return {textWidth, textHeight};
    }

    void paint(Context &ctx) const {
        auto m = _style.font.metrics();
        auto baseline = m.ascend;

        ctx.save();
        ctx.textFont(_style.font);
        if (_style.color)
            ctx.fillStyle(*_style.color);

        for (auto const &line : _lines) {
            for (usize b = line.blocks.start; b < line.blocks.end(); b++) {
                auto const &block = _blocks[b];

                for (usize c = block.cells.start; c < block.cells.end(); c++) {
                    auto const &cell = _cells[c];
                    ctx.fill({block.pos + cell.pos, baseline}, cell.glyph);
                }
            }
            baseline += m.linegap + m.descend + m.ascend;
        }
        ctx.restore();
    }
};

} // namespace Karm::Gfx
