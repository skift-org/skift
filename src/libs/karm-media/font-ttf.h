#pragma once

#include <karm-math/rand.h>
#include <karm-sys/mmap.h>
#include <ttf/spec.h>

#include "font.h"

namespace Karm::Media {

struct TtfFont : public Font {
    double _size = 18;
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
            .ascend = m.ascend * _size,
            .captop = m.ascend * _size,
            .descend = m.descend * _size,
            .linegap = m.linegap * _size,
        };
    }

    double advance(Rune c) const override {
        return _ttf.glyphMetrics(c).advance * _size;
    }

    void fillRune(Gfx::Context &g, Math::Vec2i baseline, Rune rune) const override {
        auto m = _ttf.glyphMetrics(rune);

        m.x *= _size;
        m.y *= _size;
        m.width *= _size;
        m.height *= _size;
        m.lsb *= _size;

        g.begin();
        _ttf.glyphContour(rune, g, baseline.cast<double>(), _size);
        g.fill();
    }

    void strokeRune(Gfx::Context &g, Math::Vec2i baseline, Rune rune) const override {
        auto m = _ttf.glyphMetrics(rune);

        m.x *= _size;
        m.y *= _size;
        m.width *= _size;
        m.height *= _size;
        m.lsb *= _size;

        g.begin();
        _ttf.glyphContour(rune, g, baseline.cast<double>(), _size);
        g.stroke();
    }
};

} // namespace Karm::Media
