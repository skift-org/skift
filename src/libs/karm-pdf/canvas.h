#pragma once

#include <karm-gfx/canvas.h>
#include <karm-io/emit.h>
#include <karm-io/impls.h>

namespace Karm::Pdf {

struct Canvas : public Gfx::Canvas {
    Io::StringWriter _buf;
    Math::Vec2f _p;

    Io::Emit emit() {
        return _buf;
    }

    Math::Vec2f _mapPoint(Math::Vec2f p, Math::Path::Flags flags) {
        if (flags & Math::Path::RELATIVE)
            return p;
        return _p + p;
    }

    Math::Vec2f _mapPointAndUpdate(Math::Vec2f p, Math::Path::Flags flags) {
        return _p = _mapPoint(p, flags);
    }

    void beginPath() override;

    void closePath() override;

    void moveTo(Math::Vec2f p, Math::Path::Flags flags) override;

    void lineTo(Math::Vec2f p, Math::Path::Flags flags) override;

    void hlineTo(f64 x, Math::Path::Flags flags) override;

    void vlineTo(f64 y, Math::Path::Flags flags) override;

    void cubicTo(Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Math::Path::Flags flags) override;

    void quadTo(Math::Vec2f cp, Math::Vec2f p, Math::Path::Flags flags) override;

    void arcTo(Math::Vec2f radii, f64 angle, Math::Vec2f p, Math::Path::Flags flags) override;

    void line(Math::Edgef line) override;

    void curve(Math::Curvef curve) override;

    void ellipse(Math::Ellipsef ellipse) override;

    void rect(Math::Rectf rect, Math::Radiif radii = 0) override;

    void fill(Gfx::Fill fill, Gfx::FillRule rule) override;

    void stroke(Gfx::Stroke style) override;

    Bytes bytes() const {
        return _buf.bytes();
    }
};

} // namespace Karm::Pdf
