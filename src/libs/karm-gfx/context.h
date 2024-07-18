#pragma once

#include "buffer.h"
#include "filters.h"
#include "paint.h"
#include "path.h"
#include "rast.h"
#include "stroke.h"

namespace Karm::Gfx {

struct LcdLayout {
    Math::Vec2f red;
    Math::Vec2f green;
    Math::Vec2f blue;
};

static LcdLayout RGB = {{+0.33, 0.0}, {0.0, 0.0}, {-0.33, 0.0}};
static LcdLayout BGR = {{-0.33, 0.0}, {0.0, 0.0}, {+0.33, 0.0}};
static LcdLayout VRGB = {{0.0, +0.33}, {0.0, 0.0}, {0.0, -0.33}};

struct Context {
    struct Scope {
        Paint paint = Gfx::WHITE;
        Stroke stroke{};
        Math::Recti clip{};
        Math::Trans2f trans = Math::Trans2f::identity();
    };

    Opt<MutPixels> _pixels{};
    Vec<Scope> _stack{};
    Path _path{};
    Math::Polyf _poly;
    Rast _rast{};
    LcdLayout _lcdLayout = RGB;
    bool _useSpaa = false;

    // MARK: Scope -------------------------------------------------------------

    // Begin drawing operations on the given pixels.
    void begin(MutPixels p);

    // End drawing operations.
    void end();

    // Get the pixels being drawn on.
    MutPixels mutPixels();

    // Get the pxeils being drawn on.
    Pixels pixels() const;

    // Get the current scope.
    Scope &current();

    // Get the current scope.
    Scope const &current() const;

    // Push a new scope.
    void save();

    // Pop the current scope.
    void restore();

    // A closure that receives a new Context as input.This context represents a
    // new transparency layer that you can draw into.When the closure returns,
    // karm-ui draws the new layer into the current context.
    void layer(auto inner) {
        layer({}, inner);
    }

    // A closure that receives a new Context as input.This context represents a
    // new transparency layer that you can draw into.When the closure returns,
    // karm-ui draws the new layer into the current context.
    void layer(Math::Vec2i offset, auto inner) {
        auto old = mutPixels();
        auto layer = Surface::alloc(
            pixels().size(),
            pixels().fmt()
        );

        _pixels = layer->mutPixels();
        inner(*this);
        _pixels = old;
        blit(offset, layer->pixels());
    }

    // MARK: Origin & Clipping -------------------------------------------------

    // Set the current clipping rectangle.
    void clip(Math::Recti rect);

    // Set the current origin.
    void origin(Math::Vec2i pos);

    // MARK: Transform ---------------------------------------------------------

    // Transform subsequent drawing operations using the given matrix.
    void transform(Math::Trans2f trans);

    // Translate subsequent drawing operations.
    void translate(Math::Vec2f pos);

    // Scale subsequent drawing operations.
    void scale(Math::Vec2f pos);

    // Rotate subsequent drawing operations.
    void rotate(f64 angle);

    // Skew subsequent drawing operations.
    void skew(Math::Vec2f pos);

    // Reset the transformation matrix to the identity matrix.
    void identity();

    // MARK: Fill & Stroke -----------------------------------------------------

    // Get the current fill style.
    Paint const &fillStyle();

    // Get the current stroke style.
    Stroke const &strokeStyle();

    // Set the current fill style.
    Context &fillStyle(Paint style);

    // Set the current stroke style.
    Context &strokeStyle(Stroke style);

    // MARK: Drawing -----------------------------------------------------------

    // Clear all pixels with respect to the current origin and clip.
    void clear(Color color = BLACK);

    // Clear the given rectangle with respect to the current origin and clip.
    void clear(Math::Recti rect, Color color = BLACK);

    // MARK: Blitting ----------------------------------------------------------

    void _blit(
        Pixels src,
        Math::Recti srcRect,
        auto srcFmt,

        MutPixels dest,
        Math::Recti destRect,
        auto destFmt
    );

    // Blit the given pixels to the current pixels
    // using the given source and destination rectangles.
    void blit(Math::Recti src, Math::Recti dest, Pixels pixels);

    // Blit the given pixels to the current pixels.
    // The source rectangle is the entire piels.
    void blit(Math::Recti dest, Pixels pixels);

    // Blit the given pixels to the current pixels at the given position.
    void blit(Math::Vec2i dest, Pixels pixels);

    // MARK: Shapes ------------------------------------------------------------

    // Stroke a line
    void stroke(Math::Edgef edge);

    // Fill a line with the given thickness.
    void fill(Math::Edgef edge, f64 thickness = 1.0f);

    // Stroke a rectangle.
    void stroke(Math::Rectf rect, Math::Radiusf radius = 0);

    // Fast path for filling simple rectangles without a border radius.
    void _fillRect(Math::Recti r, Gfx::Color color);

    // Fill a rectangle.
    void fill(Math::Recti rect, Math::Radiusf radius = 0);

    // Fill a rectangle.
    void fill(Math::Rectf rect, Math::Radiusf radius = 0);

    // Stroke an ellipse.
    void stroke(Math::Ellipsef e);

    // Fill an ellipse.
    void fill(Math::Ellipsef e);

    // stroke a path
    void stroke(Gfx::Path const &path);

    // fill a path
    void fill(Gfx::Path const &path, FillRule rule = FillRule::NONZERO);

    // MARK: Debug -------------------------------------------------------------
    // These functions are mostly for debugging purposes. They let you draw
    // non-antialiased shapes and lines.

    // Plot a point.
    void plot(Math::Vec2i point, Color color);

    // Draw a line.
    void plot(Math::Edgei edge, Color color);

    // Draw a rectangle.
    void plot(Math::Recti rect, Color color);

    // Draw the current path as a polygon.
    void plot(Gfx::Color color);

    // MARK: Paths -------------------------------------------------------------

    // (internal) Fill the current shape with the given paint.
    // NOTE: The shape must be flattened before calling this function.
    void _fillImpl(auto paint, auto format, FillRule fillRule);
    void _FillSmoothImpl(auto paint, auto format, FillRule fillRule);
    void _fill(Paint paint, FillRule rule = FillRule::NONZERO);

    // Begin a new path.
    void begin();

    // Close the current path. This will connect the last point to the first
    void close();

    // Begin a new subpath at the given point.
    void moveTo(Math::Vec2f p, Path::Flags flags = Path::DEFAULT);

    // Add a line segment to the current path.
    void lineTo(Math::Vec2f p, Path::Flags flags = Path::DEFAULT);

    // Add a horizontal line segment to the current path.
    void hlineTo(f64 x, Path::Flags flags = Path::DEFAULT);

    // Add a vertical line segment to the current path.
    void vlineTo(f64 y, Path::Flags flags = Path::DEFAULT);

    // Add a cubic Bezier curve to the current path.
    void cubicTo(Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Path::Flags flags = Path::DEFAULT);

    // Add a quadratic Bezier curve to the current path.
    void quadTo(Math::Vec2f cp, Math::Vec2f p, Path::Flags flags = Path::DEFAULT);

    // Add an elliptical arc to the current path.
    void arcTo(Math::Vec2f radius, f64 angle, Math::Vec2f p, Path::Flags flags = Path::DEFAULT);

    // Evaluate the given SVG path and add it to the current path.
    bool evalSvg(Str path);

    // Add a line segment to the current path.
    void line(Math::Edgef line);

    // Add a rectangle to the current path.
    void rect(Math::Rectf rect, Math::Radiusf radius = 0);

    // Add an ellipse to the current path.
    void ellipse(Math::Ellipsef ellipse);

    // Fill the current path.
    void fill(FillRule rule = FillRule::NONZERO);

    // Fill the current path with the given paint.
    void fill(Paint paint, FillRule rule = FillRule::NONZERO);

    // Stroke the current path.
    void stroke();

    // Stroke the current path with the given style.
    void stroke(Stroke style);

    // MARK: Filters -----------------------------------------------------------

    // Apply the given filter to the current pixels.
    void apply(Filter filter);

    // Apply the given filter to the given region of the current pixels.
    void apply(Filter filter, Math::Recti region);
};

} // namespace Karm::Gfx
