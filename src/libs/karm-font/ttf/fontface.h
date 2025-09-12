#pragma once

import Karm.Core;

#include <karm-gfx/canvas.h>
#include <karm-gfx/font.h>
#include <karm-sys/mmap.h>

#include "parser.h"

namespace Karm::Font::Ttf {

struct Fontface : Gfx::Fontface {
    Sys::Mmap _mmap;
    Parser _parser;
    Map<Rune, Gfx::Glyph> _cachedEntries;
    Map<Gfx::Glyph, f64> _cachedAdvances;
    Map<Pair<Gfx::Glyph>, f64> _cachedKerns;
    f64 _unitPerEm = 0;

    static Res<Rc<Fontface>> load(Sys::Mmap&& mmap);

    Fontface(Sys::Mmap&& mmap, Parser parser);

    Gfx::FontMetrics metrics() override;

    Gfx::FontAttrs attrs() const override;

    Gfx::Glyph glyph(Rune rune) override;

    f64 advance(Gfx::Glyph glyph) override;

    f64 kern(Gfx::Glyph prev, Gfx::Glyph curr) override;

    void contour(Gfx::Canvas& g, Gfx::Glyph glyph) const override;
};

} // namespace Karm::Font::Ttf
