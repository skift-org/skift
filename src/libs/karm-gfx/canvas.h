#pragma once

#include <karm-math/path.h>
#include <karm-meta/nocopy.h>
#include <karm-text/base.h>

#include "fill.h"
#include "filters.h"
#include "stroke.h"
#include "types.h"

namespace Karm::Gfx {

struct Canvas : Meta::NoCopy {
    // NOTE: Canvas is marked as NoCopy because it doesn't make sense to copy
    // a canvas. And it's also a good way to prevent accidental copies.

    Canvas() = default;

    Canvas(Canvas&&) = default;
    Canvas& operator=(Canvas&&) = default;

    virtual ~Canvas() = default;

    // MARK: Context Operations ------------------------------------------------

    // Push the current context onto the stack.
    virtual void push() = 0;

    // Pop the current context from the stack.
    virtual void pop() = 0;

    // Set the current fill style.
    virtual void fillStyle(Fill style) = 0;

    // Set the current stroke style.
    virtual void strokeStyle(Stroke style) = 0;

    // Set the origin of the current context.
    virtual void origin(Math::Vec2f p);

    // Transform subsequent drawing operations using the given matrix.
    virtual void transform(Math::Trans2f trans) = 0;

    // Translate subsequent drawing operations.
    virtual void translate(Math::Vec2f pos);

    // Scale subsequent drawing operations.
    virtual void scale(Math::Vec2f pos);

    // Rotate subsequent drawing operations.
    virtual void rotate(f64 angle);

    // Skew subsequent drawing operations.
    virtual void skew(Math::Vec2f pos);

    // MARK: Path Operations ---------------------------------------------------

    // Begin a new path.
    virtual void beginPath() = 0;

    // Close the current path. This will connect the last point to the first
    virtual void closePath() = 0;

    // Begin a new subpath at the given point.
    virtual void moveTo(Math::Vec2f p, Math::Path::Flags flags = Math::Path::DEFAULT) = 0;

    // Add a line segment to the current path.
    virtual void lineTo(Math::Vec2f p, Math::Path::Flags flags = Math::Path::DEFAULT) = 0;

    // Add a horizontal line segment to the current path.
    virtual void hlineTo(f64 x, Math::Path::Flags flags = Math::Path::DEFAULT) = 0;

    // Add a vertical line segment to the current path.
    virtual void vlineTo(f64 y, Math::Path::Flags flags = Math::Path::DEFAULT) = 0;

    // Add a cubic Bezier curve to the current path.
    virtual void cubicTo(Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Math::Path::Flags flags = Math::Path::DEFAULT) = 0;

    // Add a quadratic Bezier curve to the current path.
    virtual void quadTo(Math::Vec2f cp, Math::Vec2f p, Math::Path::Flags flags = Math::Path::DEFAULT) = 0;

    // Add an elliptical arc to the current path.
    virtual void arcTo(Math::Vec2f radius, f64 angle, Math::Vec2f p, Math::Path::Flags flags = Math::Path::DEFAULT) = 0;

    // Add a line segment to the current path.
    virtual void line(Math::Edgef line) = 0;

    // Add a curve to the current path.
    virtual void curve(Math::Curvef curve) = 0;

    // Add a rectangle to the current path.
    virtual void rect(Math::Rectf rect, Math::Radiif radii = 0) = 0;

    // Add an ellipse to the current path.
    virtual void ellipse(Math::Ellipsef ellipse) = 0;

    // Add an arc to the current path.
    virtual void arc(Math::Arcf arc) = 0;

    // Add a path to the current path.
    virtual void path(Math::Path const& path) = 0;

    // Fill the current path with the given fill.
    virtual void fill(FillRule rule = FillRule::NONZERO) = 0;

    // Change the current fill style and fill the current path.
    virtual void fill(Fill style, FillRule rule = FillRule::NONZERO);

    // Stroke the current path with the given style.
    virtual void stroke() = 0;

    // Change the current stroke style and stroke the current path.
    virtual void stroke(Stroke style);

    // Clip the current context to the current path.
    virtual void clip(FillRule rule = FillRule::NONZERO) = 0;

    // Apply a filter on the current path.
    virtual void apply(Filter filter) = 0;

    // MARK: Shape Operations --------------------------------------------------

    // This section defines a set of virtual functions for drawing and filling
    // basic shapes (lines, rectangles, ellipses) and complex paths. Each graphics
    // backend should provide its own implementation tailored to its technology.

    // NOTE: Optimization Strategy
    //       All functions operate **as if** they clear the current path, start a new one,
    //       add the shape, and then fill or stroke it. This enables backends to batch
    //       operations for potential performance gains.

    // Stroke a line
    virtual void stroke(Math::Edgef edge);

    // Stroke a rectangle.
    virtual void stroke(Math::Rectf r, Math::Radiif radii = 0);

    // Fill a rectangle.
    virtual void fill(Math::Rectf r, Math::Radiif radii = 0);

    // Fill a rectangle, with integer coordinates.
    // NOTE: This is a convenience function for backends that have
    //       optimized paths for integer coordinates.
    virtual void fill(Math::Recti r, Math::Radiif radii = 0);

    // Clip the current context to the given rectangle.
    virtual void clip(Math::Recti r);

    // Clip the current context to the given rectangle.
    virtual void clip(Math::Rectf r);

    // Stroke an ellipse.
    virtual void stroke(Math::Ellipsef e);

    // Fill an ellipse.
    virtual void fill(Math::Ellipsef e);

    // stroke a path
    virtual void stroke(Math::Path const& p);

    // fill a path
    virtual void fill(Math::Path const& p, FillRule rule = FillRule::NONZERO);

    // Fill a single glyph of text
    virtual void fill(Text::Font& font, Text::Glyph glyph, Math::Vec2f baseline);

    // Fill a run of text
    virtual void fill(Text::Prose& prose);

    // MARK: Clear Operations --------------------------------------------------

    // Clear all pixels with respect to the current origin and clip.
    virtual void clear(Color color = BLACK) = 0;

    // Clear the given rectangle with respect to the current origin and clip.
    virtual void clear(Math::Recti rect, Color color = BLACK) = 0;

    // MARK: Plot Operations ---------------------------------------------------

    // Plot a point.
    virtual void plot(Math::Vec2i point, Color color) = 0;

    // Draw a line.
    virtual void plot(Math::Edgei edge, Color color) = 0;

    // Draw a rectangle.
    virtual void plot(Math::Recti rect, Color color) = 0;

    // MARK: Blit Operations ---------------------------------------------------

    // Blit the given pixels to the current pixels
    // using the given source and destination rectangles.
    virtual void blit(Math::Recti src, Math::Recti dest, Pixels pixels) = 0;

    // Blit the given pixels to the current pixels.
    // The source rectangle is the entire piels.
    virtual void blit(Math::Recti dest, Pixels pixels);

    // Blit the given pixels to the current pixels at the given position.
    virtual void blit(Math::Vec2i dest, Pixels pixels);

    // MARK: Filter Operations -------------------------------------------------

    // Apply a filter on the given region.
    void apply(Filter filter, Math::Rectf region, Math::Radiif radii = 0);

    // Apply a filter on the given region.
    void apply(Filter filter, Math::Ellipsef region);

    // Apply a filter on the given region.
    void apply(Filter filter, Math::Path const& region);
};

} // namespace Karm::Gfx
