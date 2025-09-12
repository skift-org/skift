#include "prose.h"

#include "canvas.h"

namespace Karm::Gfx {

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

void Prose::append(StrutCell&& strut) {
    if (_blocks.len() and not last(_blocks).empty())
        _beginBlock();

    _strutCellsIndexes.pushBack(_cells.len());
    _cells.pushBack({
        .prose = this,
        .span = _currentSpan,
        .runeRange = {_runes.len(), 1},
        .glyph = Glyph::TOFU,
        .relatedStrutIndex = _struts.len(),
    });
    _struts.pushBack(std::move(strut));

    _runes.pushBack(0);

    last(_blocks).cellRange.size++;
    last(_blocks).runeRange.end(_runes.len());
}

void Prose::append(Rune rune) {
    if (any(_blocks) and (last(_blocks).newline() or last(_blocks).spaces() or last(_blocks).strut()))
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
    _lines.clear();

    _beginBlock();
}

void Prose::overrideSpanStackWith(Prose const& prose) {
    _spans.clear();

    for (auto currSpan = prose._currentSpan; currSpan != NONE; currSpan = currSpan.unwrap()->parent) {
        Rc<Span> copySpanRc = currSpan.unwrap();
        _spans.pushBack(copySpanRc);
    }

    reverse(mutSub(_spans));

    if (_spans.len()) {
        auto lastSpan = last(_spans);
        _currentSpan = lastSpan;
    }
}

void Prose::append(Slice<Rune> runes) {
    _runes.ensure(_runes.len() + runes.len());
    for (auto rune : runes) {
        append(rune);
    }
}

// MARK: Layout -------------------------------------------------------------

void Prose::_measureBlocks() {
    for (auto& block : _blocks) {
        auto adv = 0_au;
        bool first = true;
        Glyph prev = Glyph::TOFU;
        for (auto& cell : block.cells()) {
            if (not first)
                adv += Au{_style.font.kern(prev, cell.glyph)};
            else
                first = false;

            cell.pos = adv;
            cell.measureAdvance();

            adv += cell.adv;
            prev = cell.glyph;
        }
        block.width = adv;
    }
}

void Prose::_wrapLines(Au width) {
    _lines.clear();

    Line line{this, {}, {}};
    bool first = true;
    Au adv = 0_au;
    for (usize i = 0; i < _blocks.len(); i++) {
        auto& block = _blocks[i];
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
                adv = 0_au;
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
                adv = 0_au;
            } else {
                adv += block.width;
            }
        }
        first = false;
    }

    _lines.pushBack(line);
}

Au Prose::_layoutVerticaly() {
    auto m = _style.font.metrics();

    // NOTE: applying ceiling so fonts are pixel aligned
    f64 halfFontLineGap = m.linegap / 2;
    Au fontAscent = Au{Math::ceil(m.ascend + halfFontLineGap)};
    Au fontDescend = Au{Math::ceil(m.descend + halfFontLineGap)};

    Au currHeight = 0_au;
    for (auto& line : _lines) {
        Au lineTop = currHeight;

        Au maxAscent = 0_au;
        Au maxDescend = 0_au;
        for (auto const& block : line.blocks()) {
            if (block.strut()) {
                Au baseline{block.strut()->baseline};
                maxAscent = max(maxAscent, baseline);
                maxDescend = max(maxDescend, block.strut()->size.y - baseline);
            } else {
                maxAscent = max(maxAscent, fontAscent);
                maxDescend = max(maxDescend, fontDescend);
            }
        }

        line.baseline = lineTop + maxAscent;
        currHeight += maxAscent + maxDescend;
    }

    return currHeight;
}

Au Prose::_layoutHorizontaly(Au width) {
    Au maxWidth = 0_au;
    for (auto& line : _lines) {
        if (not line.blockRange.any())
            continue;

        Au pos = 0_au;
        for (auto& block : line.blocks()) {
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
            for (auto& block : line.blocks())
                block.pos += free / 2_au;
            break;

        case TextAlign::RIGHT:
            for (auto& block : line.blocks())
                block.pos += free;
            break;
        }
    }

    return maxWidth;
}

Vec2Au Prose::layout(Au width) {
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

// MARK: Paint -----------------------------------------------------------------

void Prose::paintCaret(Gfx::Canvas& g, usize runeIndex, Gfx::Color color) {
    auto m = _style.font.metrics();
    auto baseline = queryPosition(runeIndex);
    auto cs = baseline.cast<f64>() - Math::Vec2f{0, m.ascend};
    auto ce = baseline.cast<f64>() + Math::Vec2f{0, m.descend};

    g.beginPath();
    g.moveTo(cs);
    g.lineTo(ce);
    g.strokeStyle({
        .fill = color,
        .width = 1.0,
        .align = Gfx::INSIDE_ALIGN,
    });
    g.stroke();
}

void Prose::paintSelection(Gfx::Canvas& g, usize start, usize end, Gfx::Color color) {
    if (start == end)
        return;

    if (not _style.multiline) {
        auto ps = queryPosition(start);
        auto pe = queryPosition(end);
        auto m = _style.font.metrics();

        auto rect =
            RectAu::fromTwoPoint(
                ps + Vec2Au{0_au, Au{m.descend}},
                pe - Vec2Au{0_au, Au{m.ascend}}
            )
                .cast<f64>();

        g.fillStyle(color);
        g.fill(rect);
        return;
    }
}

} // namespace Karm::Gfx
