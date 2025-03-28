#include "borders.h"

namespace Karm::Gfx {

Tuple<Math::Curvef, Math::Curvef, Math::Curvef, Math::Curvef> Borders::_computeCornerCurvers(Math::Radiif const& uncheckRadii, Math::Rectf const& rect) {
    auto radii = uncheckRadii.reduceOverlap(rect.size());

    f64 cpa = radii.a * (1 - CIRCLE_APPROXIMATION);
    f64 cpb = radii.b * (1 - CIRCLE_APPROXIMATION);
    f64 cpc = radii.c * (1 - CIRCLE_APPROXIMATION);
    f64 cpd = radii.d * (1 - CIRCLE_APPROXIMATION);
    f64 cpe = radii.e * (1 - CIRCLE_APPROXIMATION);
    f64 cpf = radii.f * (1 - CIRCLE_APPROXIMATION);
    f64 cpg = radii.g * (1 - CIRCLE_APPROXIMATION);
    f64 cph = radii.h * (1 - CIRCLE_APPROXIMATION);

    return {
        // Top start edge
        {
            {rect.x, rect.y + radii.a},
            {rect.x, rect.y + cpa},
            {rect.x + cpb, rect.y},
            {rect.x + radii.b, rect.y},
        },

        // Top end edge
        {
            {rect.x + rect.width - radii.c, rect.y},
            {rect.x + rect.width - cpc, rect.y},
            {rect.x + rect.width, rect.y + cpd},
            {rect.x + rect.width, rect.y + radii.d},
        },

        // Bottom end edge
        {
            {rect.x + rect.width, rect.y + rect.height - radii.e},
            {rect.x + rect.width, rect.y + rect.height - cpe},
            {rect.x + rect.width - cpf, rect.y + rect.height},
            {rect.x + rect.width - radii.f, rect.y + rect.height},
        },

        // Bottom start edge
        {
            {rect.x + radii.g, rect.y + rect.height},
            {rect.x + cpg, rect.y + rect.height},
            {rect.x, rect.y + rect.height - cph},
            {rect.x, rect.y + rect.height - radii.h},
        },
    };
}

void Borders::_paintCurveEdge(Gfx::Canvas& c, Pair<Math::Curvef> const& start, Pair<Math::Curvef> const& end, Gfx::Fill const& color) {
    c.beginPath();

    // Outer edge
    auto outerStart = start.v0, outerEnd = end.v0;
    c.moveTo(outerStart.a);
    c.cubicTo(outerStart.b, outerStart.c, outerStart.d);

    c.lineTo(outerEnd.a);
    c.cubicTo(outerEnd.b, outerEnd.c, outerEnd.d);

    // Inner edge
    auto innerStart = start.v1, innerEnd = end.v1;
    c.lineTo(innerEnd.d);
    c.cubicTo(innerEnd.c, innerEnd.b, innerEnd.a);

    c.lineTo(innerStart.d);
    c.cubicTo(innerStart.c, innerStart.b, innerStart.a);

    c.closePath();

    c.fill(color);
}

void Borders::_paintCurveEdges(Gfx::Canvas& c, Math::Rectf rect) {
    // Calculates outer rectangles and their corresponding radii.
    Math::Rectf outerRect = rect;
    Math::Radiif outerRadii = radii;
    outerRadii.reduceOverlap(outerRect.size());

    auto [outerTopStart, outerTopEnd, outerBottomEnd, outerBottomStart] = _computeCornerCurvers(outerRadii, outerRect);

    // Calculates inner rectangles and their corresponding radii.
    Math::Rectf innerRect = rect.shrink(widths);
    Math::Radiif innerRadii = radii.shrink(widths);

    auto [innerTopStart, innerTopEnd, innerBottomEnd, innerBottomStart] = _computeCornerCurvers(innerRadii, innerRect);

    auto topStartRatio = widths.start / (widths.top + widths.start);
    if (Math::isNan(topStartRatio))
        topStartRatio = 0.5;

    auto topEndRatio = widths.top / (widths.top + widths.end);
    if (Math::isNan(topEndRatio))
        topEndRatio = 0.5;

    auto bottomEndRatio = widths.bottom / (widths.bottom + widths.end);
    if (Math::isNan(bottomEndRatio))
        bottomEndRatio = 0.5;

    auto bottomStartRatio = widths.bottom / (widths.bottom + widths.start);
    if (Math::isNan(bottomStartRatio))
        bottomStartRatio = 0.5;

    if (styles[0] != BorderStyle::NONE)
        _paintCurveEdge(
            c,
            {
                outerTopStart.split(topStartRatio).v1,
                innerTopStart.split(topStartRatio).v1,
            },
            {
                outerTopEnd.split(topEndRatio).v0,
                innerTopEnd.split(topEndRatio).v0,
            },
            fills[0]
        );

    if (styles[1] != BorderStyle::NONE)
        _paintCurveEdge(
            c,
            {
                outerTopEnd.split(topEndRatio).v1,
                innerTopEnd.split(topEndRatio).v1,
            },
            {
                outerBottomEnd.split(bottomEndRatio).v0,
                innerBottomEnd.split(bottomEndRatio).v0,
            },
            fills[1]
        );

    if (styles[2] != BorderStyle::NONE)
        _paintCurveEdge(
            c,
            {
                outerBottomEnd.split(bottomEndRatio).v1,
                innerBottomEnd.split(bottomEndRatio).v1,
            },
            {
                outerBottomStart.split(bottomStartRatio).v0,
                innerBottomStart.split(bottomStartRatio).v0,
            },
            fills[2]
        );

    if (styles[3] != BorderStyle::NONE)
        _paintCurveEdge(
            c,
            {
                outerBottomStart.split(bottomStartRatio).v1,
                innerBottomStart.split(bottomStartRatio).v1,
            },
            {
                outerTopStart.split(topStartRatio).v0,
                innerTopStart.split(topStartRatio).v0,
            },
            fills[3]
        );
}

void Borders::_paintStraightEdge(Gfx::Canvas& c, Pair<Math::Vec2f> const& start, Pair<Math::Vec2f> const& end, Gfx::Fill const& color) {
    c.beginPath();

    // Outer edge
    c.moveTo(start.v0);
    c.lineTo(end.v0);

    // Inner edge
    c.lineTo(end.v1);
    c.lineTo(start.v1);

    c.closePath();

    c.fill(color);
}

void Borders::_paintStraightEdges(Gfx::Canvas& c, Math::Rectf rect) {
    Math::Rectf outerRect = rect;
    Math::Rectf innerRect = rect.shrink(widths);

    if (styles[0] != BorderStyle::NONE)
        _paintStraightEdge(
            c,
            {outerRect.topStart(), innerRect.topStart()},
            {outerRect.topEnd(), innerRect.topEnd()},
            fills[0]
        );

    if (styles[1] != BorderStyle::NONE)
        _paintStraightEdge(
            c,
            {outerRect.topEnd(), innerRect.topEnd()},
            {outerRect.bottomEnd(), innerRect.bottomEnd()},
            fills[1]
        );

    if (styles[2] != BorderStyle::NONE)
        _paintStraightEdge(
            c,
            {outerRect.bottomEnd(), innerRect.bottomEnd()},
            {outerRect.bottomStart(), innerRect.bottomStart()},
            fills[2]
        );

    if (styles[3] != BorderStyle::NONE)
        _paintStraightEdge(
            c,
            {outerRect.bottomStart(), innerRect.bottomStart()},
            {outerRect.topStart(), innerRect.topStart()},
            fills[3]
        );
}

void Borders::_paintSimpleEdges(Gfx::Canvas& c, Math::Rectf rect) {
    Math::Rectf outerRect = rect;
    Math::Radiif outerRadii = radii;
    outerRadii.reduceOverlap(outerRect.size());
    Math::Rectf innerRect = rect.shrink(widths);
    Math::Radiif innerRadii = radii.shrink(widths);

    c.beginPath();
    c.rect(outerRect, outerRadii);
    c.rect(innerRect, innerRadii);
    c.fill(fills[0], FillRule::EVENODD);
}

bool Borders::_isSimple() {
    // A border is simple if the four sides are solid and have the same color.
    if (styles[0] != BorderStyle::SOLID or
        styles[1] != BorderStyle::SOLID or
        styles[2] != BorderStyle::SOLID or
        styles[3] != BorderStyle::SOLID)
        return false;

    auto color = fills[0].is<Color>();
    if (not color)
        return false;

    if (fills[1] != *color or
        fills[2] != *color or
        fills[3] != *color)
        return false;

    return true;
}

void Borders::paint(Gfx::Canvas& c, Math::Rectf rect) {
    if (widths.zero())
        return;

    if (_isSimple()) {
        _paintSimpleEdges(c, rect);
        return;
    }

    if (radii.zero()) {
        _paintStraightEdges(c, rect);
        return;
    }

    _paintCurveEdges(c, rect);
}

} // namespace Karm::Gfx
