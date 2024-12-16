#include "prose.h"

namespace Karm::Text {

Prose::Prose(ProseStyle style, Str str) : _style(style) {
    clear();
    _spaceWidth = _style.font.advance(_style.font.glyph(' '));
    auto m = _style.font.metrics();
    _lineHeight = m.ascend;
    append(str);
}

// MARK: Prose --------------------------------------------------------------

void Prose::_beginBlock() {
    _blocks.pushBack({
        .prose = this,
        .runeRange = {_runes.len(), 0},
        .cellRange = {_cells.len(), 0},
    });
}

void Prose::append(Rune rune) {
    if (any(_blocks) and last(_blocks).newline())
        _beginBlock();

    if (any(_blocks) and last(_blocks).spaces())
        _beginBlock();

    auto glyph = _style.font.glyph(rune == '\n' ? ' ' : rune);

    _cells.pushBack({
        .prose = this,
        .span = _currentSpan,
        .runeRange = {_runes.len(), 1},
        .glyph = glyph,
    });

    _runes.pushBack(rune);
    last(_blocks).cellRange.size++;
    last(_blocks).runeRange.end(_runes.len());
}

void Prose::clear() {
    _runes.clear();
    _cells.clear();
    _blocks.clear();
    _blocksMeasured = false;
    _beginBlock();
    _lines.clear();
}

void Prose::append(Slice<Rune> runes) {
    _runes.ensure(_runes.len() + runes.len());
    for (auto rune : runes) {
        append(rune);
    }
}

// MARK: Layout -------------------------------------------------------------

void Prose::_measureBlocks() {
    for (auto &block : _blocks) {
        auto adv = 0.0f;
        bool first = true;
        Glyph prev = Glyph::TOFU;
        for (auto &cell : block.cells()) {
            if (not first)
                adv += _style.font.kern(prev, cell.glyph);
            else
                first = false;

            cell.pos = adv;
            cell.adv = _style.font.advance(cell.glyph);
            adv += cell.adv;
            prev = cell.glyph;
        }
        block.width = adv;
    }
}

void Prose::_wrapLines(f64 width) {
    _lines.clear();

    Line line{this, {}, {}};
    bool first = true;
    f64 adv = 0;
    for (usize i = 0; i < _blocks.len(); i++) {
        auto &block = _blocks[i];
        if (adv + block.width > width and _style.wordwrap and _style.multiline and not first) {
            _lines.pushBack(line);
            line = {this, block.runeRange, {i, 1}};
            adv = block.width;

            if (block.newline()) {
                _lines.pushBack(line);
                line = {
                    this,
                    {block.runeRange.end(), 0},
                    {i + 1, 0},
                };
                adv = 0;
            }
        } else {
            line.blockRange.size++;
            line.runeRange.end(block.runeRange.end());

            if (block.newline() and _style.multiline) {
                _lines.pushBack(line);
                line = {
                    this,
                    {block.runeRange.end(), 0},
                    {i + 1, 0},
                };
                adv = 0;
            } else {
                adv += block.width;
            }
        }
        first = false;
    }

    _lines.pushBack(line);
}

f64 Prose::_layoutVerticaly() {
    auto m = _style.font.metrics();
    f64 baseline = m.linegap / 2;
    for (auto &line : _lines) {
        baseline += m.ascend;
        line.baseline = baseline;
        baseline += m.linegap + m.descend;
    }
    return baseline - m.linegap / 2;
}

f64 Prose::_layoutHorizontaly(f64 width) {
    f64 maxWidth = 0;
    for (auto &line : _lines) {
        if (not line.blockRange.any())
            continue;

        f64 pos = 0;
        for (auto &block : line.blocks()) {
            block.pos = pos;
            pos += block.width;
        }

        auto lastBlock = _blocks[line.blockRange.end() - 1];
        line.width = lastBlock.pos + lastBlock.width;
        maxWidth = max(maxWidth, line.width);
        auto free = width - line.width;

        switch (_style.align) {
        case TextAlign::LEFT:
            break;

        case TextAlign::CENTER:
            for (auto &block : line.blocks())
                block.pos += free / 2;
            break;

        case TextAlign::RIGHT:
            for (auto &block : line.blocks())
                block.pos += free;
            break;
        }
    }

    return maxWidth;
}

Math::Vec2f Prose::layout(f64 width) {
    if (isEmpty(_blocks))
        return {};

    // Blocks measurements can be reused between layouts changes
    // only line wrapping need to be re-done
    if (not _blocksMeasured) {
        _measureBlocks();
        _blocksMeasured = true;
    }

    _wrapLines(width);
    auto textHeight = _layoutVerticaly();
    auto textWidth = _layoutHorizontaly(width);
    _size = {textWidth, textHeight};
    return {textWidth, textHeight};
}

// MARK: Paint -------------------------------------------------------------

void Prose::paint(Gfx::Canvas &g) {
    g.push();

    if (_style.color)
        g.fillStyle(*_style.color);

    for (auto const &line : _lines) {
        for (auto &block : line.blocks()) {
            for (auto &cell : block.cells()) {
                if (cell.span and cell.span->color) {
                    g.push();
                    g.fillStyle(*cell.span->color);
                    g.fill(_style.font, cell.glyph, {block.pos + cell.pos, line.baseline});
                    g.pop();
                } else {
                    g.fill(_style.font, cell.glyph, {block.pos + cell.pos, line.baseline});
                }
            }
        }
    }

    g.pop();
}

} // namespace Karm::Text
