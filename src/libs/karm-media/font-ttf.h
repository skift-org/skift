#pragma once

#include <karm-math/rand.h>
#include <karm-sys/mmap.h>
#include <ttf/spec.h>

#include "font.h"

namespace Karm::Media {

struct TtfFont : public Font {
    double _scale = 64;
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
            .ascend = m.ascend * _scale,
            .captop = m.ascend * _scale,
            .descend = m.descend * _scale,
            .linegap = m.linegap * _scale,
        };
    }

    double advance(Rune c) const override {
        return _ttf.glyphMetrics(c).advance * _scale;
    }

    void fillRune(Gfx::Context &g, Math::Vec2i baseline, Rune rune) const override {
        auto m = _ttf.glyphMetrics(rune);

        m.x *= _scale;
        m.y *= _scale;
        m.width *= _scale;
        m.height *= _scale;
        m.lsb *= _scale;

        _ttf.glyphContour(rune, g, baseline.cast<double>(), _scale);
        Gfx::createSolid(g._shape, g._path);
        g._fill(g.textStyle().color);
    }

    void strokeRune(Gfx::Context &g, Math::Vec2i baseline, Rune rune) const override {
        auto m = _ttf.glyphMetrics(rune);

        m.x *= _scale;
        m.y *= _scale;
        m.width *= _scale;
        m.height *= _scale;
        m.lsb *= _scale;

        _ttf.glyphContour(rune, g, baseline.cast<double>(), _scale);
        g.stroke();
    }
};

} // namespace Karm::Media
