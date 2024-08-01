#pragma once

#include <karm-meta/nocopy.h>

#include "fill.h"
#include "path.h"
#include "stroke.h"
#include "types.h"

namespace Karm::Gfx {

struct Canvas : public Meta::NoCopy {
    // NOTE: Canvas is marked as NoCopy because it doesn't make sense to copy
    // a context. And it's also a good way to prevent accidental copies.

    Canvas() = default;
    Canvas(Canvas &&) = default;
    Canvas &operator=(Canvas &&) = default;

    virtual ~Canvas() = default;

    // Begin a new path.
    virtual void begin() = 0;

    // Close the current path. This will connect the last point to the first
    virtual void close() = 0;

    // Begin a new subpath at the given point.
    virtual void moveTo(Math::Vec2f p, Path::Flags flags = Path::DEFAULT) = 0;

    // Add a line segment to the current path.
    virtual void lineTo(Math::Vec2f p, Path::Flags flags = Path::DEFAULT) = 0;

    // Add a horizontal line segment to the current path.
    virtual void hlineTo(f64 x, Path::Flags flags = Path::DEFAULT) = 0;

    // Add a vertical line segment to the current path.
    virtual void vlineTo(f64 y, Path::Flags flags = Path::DEFAULT) = 0;

    // Add a cubic Bezier curve to the current path.
    virtual void cubicTo(Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Path::Flags flags = Path::DEFAULT) = 0;

    // Add a quadratic Bezier curve to the current path.
    virtual void quadTo(Math::Vec2f cp, Math::Vec2f p, Path::Flags flags = Path::DEFAULT) = 0;

    // Add an elliptical arc to the current path.
    virtual void arcTo(Math::Vec2f radius, f64 angle, Math::Vec2f p, Path::Flags flags = Path::DEFAULT) = 0;

    // Add a line segment to the current path.
    virtual void line(Math::Edgef line) = 0;

    // Add a curve to the current path.
    virtual void curve(Math::Curvef curve) = 0;

    // Add a rectangle to the current path.
    virtual void rect(Math::Rectf rect, Math::Radiif radii = 0) = 0;

    // Add an ellipse to the current path.
    virtual void ellipse(Math::Ellipsef ellipse) = 0;

    // Fill the current path with the given fill.
    virtual void fill(Fill fill, FillRule rule = FillRule::NONZERO) = 0;

    // Stroke the current path with the given style.
    virtual void stroke(Stroke style) = 0;
};

} // namespace Karm::Gfx
