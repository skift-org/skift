#pragma once

#include <karm-base/map.h>
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

    static Res<Strong<TtfFontface>> load(Sys::Mmap &&mmap);

    TtfFontface(Sys::Mmap &&mmap, Ttf::Parser parser);

    FontMetrics metrics() const override;

    FontAttrs attrs() const override;

    Glyph glyph(Rune rune) override;

    f64 advance(Glyph glyph) override;

    f64 kern(Glyph prev, Glyph curr) override;

    void contour(Gfx::Canvas &g, Glyph glyph) const override;
};

} // namespace Karm::Text
