#pragma once

#include <karm-gfx/canvas.h>
#include <karm-io/emit.h>
#include <karm-io/impls.h>

namespace Karm::Pdf {

struct Canvas : public Gfx::Canvas {
    Io::Emit _e;
    Math::Vec2f _mediaBox{};
    Math::Vec2f _p{};

    Canvas(Io::Emit e, Math::Vec2f mediaBox)
        : _e{e}, _mediaBox{mediaBox} {}

    Math::Vec2f _mapPoint(Math::Vec2f p, Math::Path::Flags flags) {
        if (flags & Math::Path::RELATIVE)
            return p + _p;
        return p;
    }

    Math::Vec2f _mapPointAndUpdate(Math::Vec2f p, Math::Path::Flags flags) {
        if (flags & Math::Path::RELATIVE)
            p = p + _p;
        _p = p;
        return p;
    }

    // MARK: Context Operations ------------------------------------------------

    void push() override;

    void pop() override;

    void fillStyle(Gfx::Fill) override;

    void strokeStyle(Gfx::Stroke) override;

    void transform(Math::Trans2f trans) override;

    // MARK: Path Operations ---------------------------------------------------

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

    void path(Math::Path const& path) override;

    void fill(Gfx::FillRule rule) override;

    void fill(Gfx::Fill fill, Gfx::FillRule rule) override;

    void stroke() override;

    void stroke(Gfx::Stroke style) override;

    void clip(Gfx::FillRule rule) override;

    void apply(Gfx::Filter filter) override;

    // MARK: Clear Operations --------------------------------------------------

    void clear(Gfx::Color color) override;

    void clear(Math::Recti rect, Gfx::Color color) override;

    // MARK: Plot Operations ---------------------------------------------------

    void plot(Math::Vec2i point, Gfx::Color color) override;

    void plot(Math::Edgei edge, Gfx::Color color) override;

    void plot(Math::Recti rect, Gfx::Color color) override;

    // MARK: Blit Operations ---------------------------------------------------

    void blit(Math::Recti src, Math::Recti dest, Gfx::Pixels pixels) override;

    // MARK: Filter Operations -------------------------------------------------
};

} // namespace Karm::Pdf
