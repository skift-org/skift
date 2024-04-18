#include "text.h"

namespace Karm::Gfx {

Text::Text(TextStyle style, Str str) : _style(style) {
    clear();
    _spaceWidth = _style.font.advance(_style.font.glyph(' '));
    auto m = _style.font.metrics();
    _lineHeight = m.ascend;
    append(str);
}

// MARK: Text --------------------------------------------------------------

void Text::_beginBlock() {
    _blocks.pushBack({
        .cells = {_cells.len(), 0},
    });
}

void Text::append(Rune rune) {
    if (last(_blocks).newline)
        _beginBlock();

    if (rune == ' ') {
        last(_blocks).spaces++;
    } else if (rune == '\t') {
        // HACK: tab width is 4 spaces
        last(_blocks).spaces += 4;
    } else if (rune == '\n') {
        last(_blocks).newline = true;
    } else if (not isAsciiPrint(rune)) {
        // ignore
    } else {
        if (last(_blocks).hasWhitespace())
            _beginBlock();

        auto glyph = _style.font.glyph(rune);
        _cells.pushBack({
            .glyph = glyph,
        });
        last(_blocks).cells.size++;
    }
}

void Text::clear() {
    _cells.clear();
    _blocks.clear();
    _blocksMeasured = false;
    _beginBlock();
    _lines.clear();
}

void Text::append(Slice<Rune> runes) {
    for (auto rune : runes) {
        append(rune);
    }
}

// MARK: Layout -------------------------------------------------------------

void Text::_measureBlocks() {
    for (auto &block : _blocks) {
        auto adv = 0.0f;
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

void Text::_wrapLines(f64 width) {
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

f64 Text::_layoutVerticaly() {
    auto m = _style.font.metrics();
    f64 baseline = m.linegap / 2;
    for (auto &line : _lines) {
        baseline += m.ascend;
        line.baseline = baseline;
        baseline += m.linegap + m.descend;
    }
    return baseline - m.linegap / 2;
}

f64 Text::_layoutHorizontaly(f64 width) {
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

Math::Vec2f Text::layout(f64 width) {
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
    return {textWidth, textHeight};
}

// MARK: Paint -------------------------------------------------------------

void Text::paint(Context &ctx) const {
    auto m = _style.font.metrics();

    ctx.save();
    ctx.textFont(_style.font);
    if (_style.color)
        ctx.fillStyle(*_style.color);

    auto clip = ctx.clip();
    auto origin = ctx.origin();

    auto si =
        tryOr(searchLowerBound(_lines, [&](auto &line) {
                  return line.baseline + origin.y <=> clip.top() - m.ascend;
              }),
              0);

    auto ei = tryOr(searchUpperBound(_lines, [&](auto &line) {
                        return line.baseline + origin.y <=> clip.bottom() + m.lineheight();
                    }),
                    _lines.len());

    for (usize i = si; i < ei; i++) {
        auto const &line = _lines[i];
        for (usize b = line.blocks.start; b < line.blocks.end(); b++) {
            auto const &block = _blocks[b];

            for (usize c = block.cells.start; c < block.cells.end(); c++) {
                auto const &cell = _cells[c];
                ctx.fill({block.pos + cell.pos, line.baseline}, cell.glyph);
            }
        }
    }

    ctx.restore();
}

// MARK: Query -------------------------------------------------------------

} // namespace Karm::Gfx
