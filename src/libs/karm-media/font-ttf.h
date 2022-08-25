#pragma once

#include <karm-math/rand.h>
#include <karm-sys/mmap.h>
#include <ttf/spec.h>

#include "font.h"

namespace Karm::Media {

struct TtfFont : public Fontface {
    Sys::Mmap _mmap;
    Ttf::Font _ttf;

    static Result<Strong<TtfFont>> load(Sys::Mmap mmap) {
        auto ttf = try$(Ttf::Font::load(mmap.bytes()));
        return makeStrong<TtfFont>(std::move(mmap), ttf);
    }

    TtfFont(Sys::Mmap mmap, Ttf::Font ttf)
        : _mmap{std::move(mmap)},
          _ttf{std::move(ttf)} {
    }

    FontMetrics metrics() const override {
        auto m = _ttf.metrics();
        return {
            .ascend = m.ascend,
            .captop = m.ascend,
            .descend = m.descend,
            .linegap = m.linegap,
        };
    }

    double advance(Rune c) const override {
        return _ttf.glyphMetrics(c).advance;
    }

    void contour(Gfx::Context &g, Rune rune) const override {
        _ttf.glyphContour(g, rune);
    }

    double units() const override {
        return _ttf.unitPerEm();
    }
};

} // namespace Karm::Media
