#include "fontface.h"

namespace Karm::Font::Ttf {

Res<Rc<Fontface>> Fontface::load(Sys::Mmap&& mmap) {
    auto ttf = try$(Ttf::Parser::init(mmap.bytes()));
    return Ok(makeRc<Fontface>(std::move(mmap), ttf));
}

Fontface::Fontface(Sys::Mmap&& mmap, Ttf::Parser parser)
    : _mmap(std::move(mmap)),
      _parser(std::move(parser)) {
    _unitPerEm = _parser.unitPerEm();
}

Gfx::FontMetrics Fontface::metrics() {
    auto m = _parser.metrics();
    auto xHeight = _parser.glyphMetrics(glyph('x')).y;
    return {
        .ascend = m.ascend / _unitPerEm,
        .captop = m.ascend / _unitPerEm,
        .descend = m.descend / _unitPerEm,
        .linegap = m.linegap / _unitPerEm,
        .advance = 0,
        .xHeight = xHeight / _unitPerEm,
    };
}

Gfx::FontAttrs Fontface::attrs() const {
    Gfx::FontAttrs attrs;

    if (_parser._name.present()) {
        auto name = _parser._name;
        attrs.family = Symbol::from(name.string(name.lookupRecord(Ttf::Name::FAMILY)));
    }

    if (_parser._post.present()) {
        if (_parser._post.isFixedPitch())
            attrs.monospace = Gfx::Monospace::YES;

        if (_parser._post.italicAngle() != 0)
            attrs.style = Gfx::FontStyle::ITALIC;
    }

    if (_parser._os2.present()) {
        attrs.weight = Gfx::FontWeight{_parser._os2.weightClass()};
        attrs.stretch = Gfx::FontStretch{static_cast<u16>(_parser._os2.widthClass() * 100)};
    }

    return attrs;
}

Gfx::Glyph Fontface::glyph(Rune rune) {
    auto glyph = _cachedEntries.tryGet(rune);
    if (glyph.has())
        return glyph.unwrap();
    auto g = _parser.glyph(rune);
    _cachedEntries.put(rune, g);
    return g;
}

f64 Fontface::advance(Gfx::Glyph glyph) {
    auto advance = _cachedAdvances.tryGet(glyph);
    if (advance.has())
        return advance.unwrap();
    auto a = _parser.glyphMetrics(glyph).advance / _unitPerEm;
    _cachedAdvances.put(glyph, a);
    return a;
}

f64 Fontface::kern(Gfx::Glyph prev, Gfx::Glyph curr) {
    auto kern = _cachedKerns.tryGet({prev, curr});
    if (kern.has())
        return kern.unwrap();

    auto k = _parser.glyphKern(prev, curr) / _unitPerEm;
    _cachedKerns.put({prev, curr}, k);
    return k;
}

void Fontface::contour(Gfx::Canvas& g, Gfx::Glyph glyph) const {
    g.scale(1.0 / _unitPerEm);
    _parser.glyphContour(g, glyph);
}
} // namespace Karm::Font::Ttf
