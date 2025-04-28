#pragma once

#include "../buffer.h"
#include "../canvas.h"
#include "../fill.h"
#include "../filters.h"
#include "../stroke.h"
#include "rast.h"

namespace Karm::Gfx {

struct LcdLayout {
    Math::Vec2f red;
    Math::Vec2f green;
    Math::Vec2f blue;
};

static LcdLayout RGB = {{+0.33, 0.0}, {0.0, 0.0}, {-0.33, 0.0}};
static LcdLayout BGR = {{-0.33, 0.0}, {0.0, 0.0}, {+0.33, 0.0}};
static LcdLayout VRGB = {{0.0, +0.33}, {0.0, 0.0}, {0.0, -0.33}};

struct CpuCanvas : Canvas {
    struct Scope {
        Fill fill = Gfx::WHITE;
        Stroke stroke{};
        Math::Recti clip{};
        Math::Trans2f trans = Math::Trans2f::IDENTITY;
    };

    Opt<MutPixels> _pixels{};
    Vec<Scope> _stack{};
    Math::Path _path{};
    Math::Polyf _poly;
    CpuRast _rast{};
    LcdLayout _lcdLayout = RGB;
    bool _useSpaa = false;

    // MARK: Buffers -----------------------------------------------------------

    // Begin drawing operations on the given pixels.
    void begin(MutPixels p);

    // End drawing operations.
    void end();

    // Get the pixels being drawn on.
    MutPixels mutPixels();

    // Get the pixels being drawn on.
    Pixels pixels() const;

    // Get the current scope.
    Scope& current();

    // Get the current scope.
    Scope const& current() const;

    // MARK: Context Operations ------------------------------------------------

    void push() override;

    void pop() override;

    void fillStyle(Fill style) override;

    void strokeStyle(Stroke style) override;

    void transform(Math::Trans2f trans) override;

    // MARK: Path Operations ---------------------------------------------------

    // (internal) Fill the current shape with the given fill.
    // NOTE: The shape must be flattened before calling this function.
    void _fillImpl(auto fill, auto format, FillRule fillRule);
    void _FillSmoothImpl(auto fill, auto format, FillRule fillRule);
    void _fill(Fill fill, FillRule rule = FillRule::NONZERO);

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

    void rect(Math::Rectf rect, Math::Radiif radii) override;

    void arc(Math::Arcf arc) override;

    void path(Math::Path const& path) override;

    void ellipse(Math::Ellipsef ellipse) override;

    void fill(FillRule rule) override;

    void stroke() override;

    void clip(FillRule rule) override;

    // MARK: Shape Operations --------------------------------------------------

    void _fillRect(Math::Recti r, Gfx::Color color);

    void fill(Math::Recti rect, Math::Radiif radii) override;

    void clip(Math::Rectf rect) override;

    void stroke(Math::Path const& path) override;

    void fill(Math::Path const& path, FillRule rule = FillRule::NONZERO) override;

    void fill(Text::Font& font, Text::Glyph glyph, Math::Vec2f baseline) override;

    // MARK: Clear Operations --------------------------------------------------

    void clear(Color color = BLACK) override;

    void clear(Math::Recti rect, Color color = BLACK) override;

    // MARK: Plot Operations ---------------------------------------------------

    void plot(Math::Vec2i point, Color color) override;

    void plot(Math::Edgei edge, Color color) override;

    void plot(Math::Recti rect, Color color) override;

    // MARK: Blit Operations ---------------------------------------------------

    void _blit(
        Pixels src,
        Math::Recti srcRect,
        auto srcFmt,

        MutPixels dest,
        Math::Recti destRect,
        auto destFmt
    );

    void blit(Math::Recti src, Math::Recti dest, Pixels pixels) override;

    // MARK: Filter Operations -------------------------------------------------

    void apply(Filter filter) override;

    void apply(Filter filter, Math::Recti region);
};

} // namespace Karm::Gfx
