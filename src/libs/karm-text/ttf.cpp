#include "ttf.h"

namespace Karm::Text {

Res<Strong<TtfFontface>> TtfFontface::load(Sys::Mmap &&mmap) {
    auto ttf = try$(Ttf::Parser::init(mmap.bytes()));
    return Ok(makeStrong<TtfFontface>(std::move(mmap), ttf));
}

TtfFontface::TtfFontface(Sys::Mmap &&mmap, Ttf::Parser parser)
    : _mmap(std::move(mmap)),
      _parser(std::move(parser)) {
    _unitPerEm = _parser.unitPerEm();
}

FontMetrics TtfFontface::metrics() const {
    auto m = _parser.metrics();
    return {
        .ascend = m.ascend / _unitPerEm,
        .captop = m.ascend / _unitPerEm,
        .descend = m.descend / _unitPerEm,
        .linegap = m.linegap / _unitPerEm,
        .advance = 0
    };
}

FontAttrs TtfFontface::attrs() const {
    FontAttrs attrs;

    if (_parser._name.present()) {
        auto name = _parser._name;
        attrs.family = name.string(name.lookupRecord(Ttf::Name::FAMILY));
    }

    if (_parser._post.present()) {
        if (_parser._post.isFixedPitch())
            attrs.monospace = Monospace::YES;

        if (_parser._post.isItalic())
            attrs.style = FontStyle::ITALIC;
    }

    if (_parser._os2.present()) {
        attrs.weight = FontWeight{_parser._os2.weightClass()};
        attrs.stretch = FontStretch{static_cast<u16>(_parser._os2.widthClass() * 100)};
    }

    return attrs;
}

Glyph TtfFontface::glyph(Rune rune) {
    auto glyph = _cachedEntries.tryGet(rune);
    if (glyph.has())
        return glyph.unwrap();
    auto g = _parser.glyph(rune);
    _cachedEntries.put(rune, g);
    return g;
}

f64 TtfFontface::advance(Glyph glyph) {
    auto advance = _cachedAdvances.tryGet(glyph);
    if (advance.has())
        return advance.unwrap();
    auto a = _parser.glyphMetrics(glyph).advance / _unitPerEm;
    _cachedAdvances.put(glyph, a);
    return a;
}

f64 TtfFontface::kern(Glyph prev, Glyph curr) {
    auto kern = _cachedKerns.tryGet({prev, curr});
    if (kern.has())
        return kern.unwrap();

    auto k = _parser.glyphKern(prev, curr) / _unitPerEm;
    _cachedKerns.put({prev, curr}, k);
    return k;
}

void TtfFontface::contour(Gfx::Context &g, Glyph glyph) const {
    g.scale(1.0 / _unitPerEm);
    _parser.glyphContour(g, glyph);
}
} // namespace Karm::Text
