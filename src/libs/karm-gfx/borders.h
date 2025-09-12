#pragma once

#include <karm-math/radii.h>

#include "canvas.h"

namespace Karm::Gfx {

enum struct BorderStyle : u8 {
    NONE,

    HIDDEN,
    DOTTED,
    DASHED,
    SOLID,
    DOUBLE,
    GROOVE,
    RIDGE,
    INSET,
    OUTSET,

    _LEN
};

struct Borders {
    static constexpr f64 CIRCLE_APPROXIMATION = 0.5522847498;

    Math::Radiif radii;
    Math::Insetsf widths;
    Array<Fill, 4> fills;
    Array<BorderStyle, 4> styles;

    static Tuple<Math::Curvef, Math::Curvef, Math::Curvef, Math::Curvef> _computeCornerCurvers(Math::Radiif const& radii, Math::Rectf const& rect);

    void _paintCurveEdge(Gfx::Canvas& c, Pair<Math::Curvef> const& start, Pair<Math::Curvef> const& end, Gfx::Fill const& color);

    void _paintCurveEdges(Gfx::Canvas& c, Math::Rectf rect);

    void _paintStraightEdge(Gfx::Canvas& c, Pair<Math::Vec2f> const& start, Pair<Math::Vec2f> const& end, Gfx::Fill const& color);

    void _paintStraightEdges(Gfx::Canvas& c, Math::Rectf rect);

    void _paintSimpleEdges(Gfx::Canvas& c, Math::Rectf rect);

    bool _isSimple();

    void paint(Gfx::Canvas& c, Math::Rectf rect);

    Borders& withFill(Fill const& fill) {
        fills = {fill, fill, fill, fill};
        return *this;
    }

    Borders& withStyle(BorderStyle const& style) {
        styles = {style, style, style, style};
        return *this;
    }

    void repr(Io::Emit& e) const {
        e("(Borders radii:{} widths:{} fills:{} styles:{})", radii, widths, fills, styles);
    }
};

} // namespace Karm::Gfx
