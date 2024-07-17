#pragma once

#include <karm-base/map.h>
#include <karm-math/rand.h>
#include <karm-sys/mmap.h>

#include "font.h"
#include "ttf/parser.h"

namespace Karm::Text {

struct TtfFontface : public Fontface {
    Sys::Mmap _mmap;
    Ttf::Parser _parser;
    Map<Rune, Glyph> _cachedEntries;
    Map<Glyph, f64> _cachedAdvances;
    Map<Cons<Glyph>, f64> _cachedKerns;
    f64 _unitPerEm = 0;

    static Res<Strong<TtfFontface>> load(Sys::Mmap &&mmap) {
        auto ttf = try$(Ttf::Parser::init(mmap.bytes()));
        return Ok(makeStrong<TtfFontface>(std::move(mmap), ttf));
    }

    TtfFontface(Sys::Mmap &&mmap, Ttf::Parser parser)
        : _mmap(std::move(mmap)),
          _parser(std::move(parser)) {
        _unitPerEm = _parser.unitPerEm();
    }

    FontMetrics metrics() const override {
        auto m = _parser.metrics();
        return {
            .ascend = m.ascend / _unitPerEm,
            .captop = m.ascend / _unitPerEm,
            .descend = m.descend / _unitPerEm,
            .linegap = m.linegap / _unitPerEm,
            .advance = 0
        };
    }

    FontAttrs attrs() const override {
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

    Glyph glyph(Rune rune) override {
        auto glyph = _cachedEntries.tryGet(rune);
        if (glyph.has())
            return glyph.unwrap();
        auto g = _parser.glyph(rune);
        _cachedEntries.put(rune, g);
        return g;
    }

    f64 advance(Glyph glyph) override {
        auto advance = _cachedAdvances.tryGet(glyph);
        if (advance.has())
            return advance.unwrap();
        auto a = _parser.glyphMetrics(glyph).advance / _unitPerEm;
        _cachedAdvances.put(glyph, a);
        return a;
    }

    f64 kern(Glyph prev, Glyph curr) override {
        auto kern = _cachedKerns.tryGet({prev, curr});
        if (kern.has())
            return kern.unwrap();

        auto k = _parser.glyphKern(prev, curr) / _unitPerEm;
        _cachedKerns.put({prev, curr}, k);
        return k;
    }

    void contour(Gfx::Context &g, Glyph glyph) const override {
        g.scale(1.0 / _unitPerEm);
        _parser.glyphContour(g, glyph);
    }
};

} // namespace Karm::Text
