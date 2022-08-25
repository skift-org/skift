#pragma once

#include <karm-math/rand.h>
#include <karm-sys/mmap.h>
#include <ttf/spec.h>

#include "font.h"

namespace Karm::Media {

struct TtfFont : public Font {
    double _size = 12;
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
            .ascend = m.ascend * _size / _ttf.unitPerEm(),
            .captop = m.ascend * _size / _ttf.unitPerEm(),
            .descend = m.descend * _size / _ttf.unitPerEm(),
            .linegap = m.linegap * _size / _ttf.unitPerEm(),
        };
    }

    double advance(Rune c) const override {
        return _ttf.glyphMetrics(c).advance * _size / _ttf.unitPerEm();
    }

    void fillRune(Gfx::Context &g, Math::Vec2i baseline, Rune rune) const override {
        g.save();
        g.begin();
        g.origin(baseline);
        g.scale(_size / _ttf.unitPerEm());
        _ttf.glyphContour(g, rune);
        g.fill();
        g.restore();
    }

    void strokeRune(Gfx::Context &g, Math::Vec2i baseline, Rune rune) const override {
        g.save();
        g.begin();
        g.origin(baseline);
        g.scale(_size / _ttf.unitPerEm());
        _ttf.glyphContour(g, rune);
        g.stroke();
        g.restore();
    }
};

} // namespace Karm::Media
