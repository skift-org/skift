#pragma once

#include <karm-base/union.h>
#include <karm-base/vec.h>
#include <karm-gfx/types.h>
#include <karm-math/path.h>
#include <vaev-base/calc.h>
#include <vaev-base/length.h>
#include <vaev-base/percent.h>
#include <vaev-style/selector.h>

#include "background.h"
#include "keywords.h"

namespace Vaev {

// https://drafts.fxtf.org/css-masking/#typedef-geometry-box

using ShapeBox = FlatUnion<Keywords::BorderBox, Keywords::PaddingBox, Keywords::ContentBox, Keywords::MarginBox>;

using GeometryBox = FlatUnion<ShapeBox, Keywords::FillBox, Keywords::StrokeBox, Keywords::ViewBox>;

using FillRule = Union<Keywords::Nonzero, Keywords::Evenodd>;

inline Gfx::FillRule fillRuleToGfx(FillRule rule) {
    return rule.visit(Visitor{
        [](Keywords::Nonzero&) {
            return Gfx::FillRule::NONZERO;
        },
        [](Keywords::Evenodd&) {
            return Gfx::FillRule::EVENODD;
        },
    });
}

using ShapeRadius = Union<CalcValue<PercentOr<Length>>, Keywords::ClosestSide, Keywords::FarthestSide>;

struct Circle {
    ShapeRadius radius = Keywords::CLOSEST_SIDE;
    BackgroundPosition position = {Keywords::CENTER, Percent(0), Keywords::CENTER, Percent(0)};

    void repr(Io::Emit& e) const {
        e("(circle {} {})", radius, position);
    }
};

struct Ellipse {
    ShapeRadius rx = Keywords::CLOSEST_SIDE;
    ShapeRadius ry = Keywords::CLOSEST_SIDE;
    BackgroundPosition position = {Keywords::CENTER, Percent(0), Keywords::CENTER, Percent(0)};

    void repr(Io::Emit& e) const {
        e("(ellipse {} {} {})", rx, ry, position);
    }
};

struct Inset {
    Math::Insets<CalcValue<PercentOr<Length>>> insets = {Percent(0)};
    Math::Radii<CalcValue<PercentOr<Length>>> borderRadius = {Percent(0)};

    void repr(Io::Emit& e) const {
        e("(inset {} {})", insets, borderRadius);
    }
};

struct Path {
    Math::Path path;
    Gfx::FillRule fillRule = Gfx::FillRule::NONZERO;

    void repr(Io::Emit& e) const {
        e("(path {} {})", fillRule, path);
    }
};

struct Polygon {
    Gfx::FillRule fillRule = Gfx::FillRule::NONZERO;
    Vec<Pair<CalcValue<PercentOr<Length>>>> points;

    void repr(Io::Emit& e) const {
        e("(polygon {} {})", fillRule, points);
    }
};

struct Rect {
    Math::Insets<CalcValue<PercentOr<Length>>> insets = {Percent(0), Percent(100), Percent(100), Percent(0)};
    Math::Radii<CalcValue<PercentOr<Length>>> borderRadius = {Percent(0)};

    void repr(Io::Emit& e) const {
        e("(rect {} {})", insets, borderRadius);
    }
};

struct Xywh {
    Math::Rect<CalcValue<PercentOr<Length>>> rect = {Percent(0), Percent(0), Percent(0), Percent(0)};
    Math::Radii<CalcValue<PercentOr<Length>>> borderRadius = {Percent(0)};

    void repr(Io::Emit& e) const {
        e("(xywh {} {})", rect, borderRadius);
    }
};

using BasicShapeFunction = Union<Circle, Ellipse, Inset, Path, Polygon, Rect, Xywh>;

struct BasicShape {
    Opt<BasicShapeFunction> shape;
    GeometryBox referenceBox = Keywords::BORDER_BOX;

    void repr(Io::Emit& e) const {
        e("(basic-shape {} {})", shape, referenceBox);
    }
};

} // namespace Vaev
