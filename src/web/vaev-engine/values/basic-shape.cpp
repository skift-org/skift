module;

#include <karm-gfx/types.h>
#include <karm-math/path.h>

export module Vaev.Engine:values.basic_shape;

import Karm.Core;

import :css;
import :values.base;
import :values.calc;
import :values.length;
import :values.percent;
import :values.background;
import :values.insets;
import :values.borders;
import :values.keywords;

using namespace Karm;

namespace Vaev {

// https://drafts.fxtf.org/css-masking/#typedef-geometry-box
export using ShapeBox = FlatUnion<
    Keywords::BorderBox,
    Keywords::PaddingBox,
    Keywords::ContentBox,
    Keywords::MarginBox>;

export using GeometryBox = FlatUnion<
    ShapeBox,
    Keywords::FillBox,
    Keywords::StrokeBox,
    Keywords::ViewBox>;

export using FillRule = Union<
    Keywords::Nonzero,
    Keywords::Evenodd>;

export Gfx::FillRule fillRuleToGfx(FillRule rule) {
    return rule.visit(Visitor{
        [](Keywords::Nonzero&) {
            return Gfx::FillRule::NONZERO;
        },
        [](Keywords::Evenodd&) {
            return Gfx::FillRule::EVENODD;
        },
    });
}

export using ShapeRadius = Union<
    CalcValue<PercentOr<Length>>,
    Keywords::ClosestSide,
    Keywords::FarthestSide>;

// MARK: Circle ----------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-circle

export struct Circle {
    ShapeRadius radius = Keywords::CLOSEST_SIDE;
    BackgroundPosition position = {Keywords::CENTER, Percent(0), Keywords::CENTER, Percent(0)};

    void repr(Io::Emit& e) const {
        e("(circle {} {})", radius, position);
    }
};

export template <>
struct ValueParser<Circle> {
    static Res<Circle> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("circle(")) {
            Circle result;
            Cursor<Css::Sst> scan = c->content;

            if (auto radius = parseValue<ShapeRadius>(scan)) {
                result.radius = radius.unwrap();
            }

            eatWhitespace(scan);
            if (scan.skip(Css::Token::ident("at"))) {
                eatWhitespace(scan);
                if (auto position = parseValue<BackgroundPosition>(scan)) {
                    result.position = position.unwrap();
                }
                eatWhitespace(scan);
            }

            if (not scan.ended())
                return Error::invalidData("unexpected data in circle");

            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid circle");
    }
};

// MARK: Ellipse ---------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-ellipse

export struct Ellipse {
    ShapeRadius rx = Keywords::CLOSEST_SIDE;
    ShapeRadius ry = Keywords::CLOSEST_SIDE;
    BackgroundPosition position = {Keywords::CENTER, Percent(0), Keywords::CENTER, Percent(0)};

    void repr(Io::Emit& e) const {
        e("(ellipse {} {} {})", rx, ry, position);
    }
};

export template <>
struct ValueParser<Ellipse> {
    static Res<Ellipse> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("ellipse(")) {
            Ellipse result;
            Cursor<Css::Sst> scan = c->content;

            if (auto radius = parseValue<ShapeRadius>(scan)) {
                result.rx = radius.unwrap();
                eatWhitespace(scan);
                result.ry = try$(parseValue<ShapeRadius>(scan));
            }

            eatWhitespace(scan);
            if (scan.skip(Css::Token::ident("at"))) {
                eatWhitespace(scan);
                if (auto position = parseValue<BackgroundPosition>(scan)) {
                    result.position = position.unwrap();
                }
                eatWhitespace(scan);
            }

            if (not scan.ended())
                return Error::invalidData("unexpected data in ellipse");

            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid ellipse");
    }
};

// MARK: Inset -----------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-inset

export struct Inset {
    Math::Insets<CalcValue<PercentOr<Length>>> insets = {Percent(0)};
    Math::Radii<CalcValue<PercentOr<Length>>> borderRadius = {Percent(0)};

    void repr(Io::Emit& e) const {
        e("(inset {} {})", insets, borderRadius);
    }
};

export template <>
struct ValueParser<Inset> {
    static Res<Inset> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("inset(")) {
            Inset result;
            Cursor<Css::Sst> scan = c->content;

            if (auto insets = parseValue<Math::Insets<CalcValue<PercentOr<Length>>>>(scan)) {
                result.insets = insets.unwrap();
            } else {
                return Error::invalidData("expected insets");
            }

            eatWhitespace(scan);
            if (scan.skip(Css::Token::ident("round"))) {
                eatWhitespace(scan);
                if (auto radii = parseValue<Math::Radii<CalcValue<PercentOr<Length>>>>(scan)) {
                    result.borderRadius = radii.unwrap();
                }
                eatWhitespace(scan);
            }

            if (not scan.ended())
                return Error::invalidData("unexpected data in inset");

            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid inset");
    }
};

// MARK: Path ------------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-path

export struct Path {
    Math::Path path;
    Gfx::FillRule fillRule = Gfx::FillRule::NONZERO;

    void repr(Io::Emit& e) const {
        e("(path {} {})", fillRule, path);
    }
};

export template <>
struct ValueParser<Path> {
    static Res<Path> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("path(")) {
            Path result;
            Cursor<Css::Sst> scan = c->content;

            bool begin = true;
            if (auto fill = parseValue<FillRule>(scan)) {
                result.fillRule = fillRuleToGfx(fill.unwrap());
                begin = false;
            }

            while ((begin or scan.skip(Css::Token::COMMA)) and not scan.ended()) {
                begin = false;

                eatWhitespace(scan);
                if (not result.path.evalSvg(try$(parseValue<String>(scan))))
                    return Error::invalidData("expected svg path in a string");

                if (not last(result.path._contours).close)
                    result.path.close();
            }

            if (begin)
                return Error::invalidData("expected data in path");

            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid path");
    }
};

// MARK: Polygon ---------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-polygon

export struct Polygon {
    Gfx::FillRule fillRule = Gfx::FillRule::NONZERO;
    Vec<Pair<CalcValue<PercentOr<Length>>>> points;

    void repr(Io::Emit& e) const {
        e("(polygon {} {})", fillRule, points);
    }
};

export template <>
struct ValueParser<Polygon> {
    static Res<Polygon> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("polygon(")) {
            Polygon result;
            Cursor<Css::Sst> scan = c->content;

            bool begin = true;
            if (auto fill = parseValue<FillRule>(scan)) {
                result.fillRule = fillRuleToGfx(fill.unwrap());
                begin = false;
            }

            while ((begin or scan.skip(Css::Token::COMMA)) and not scan.ended()) {
                begin = false;

                eatWhitespace(scan);
                auto x = try$(parseValue<CalcValue<PercentOr<Length>>>(scan));
                eatWhitespace(scan);
                auto y = try$(parseValue<CalcValue<PercentOr<Length>>>(scan));
                eatWhitespace(scan);
                result.points.emplaceBack(x, y);
            }

            if (not result.points)
                return Error::invalidData("expected points in polygon");

            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid polygon");
    }
};

// MARK: Rect ------------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-rect

export struct Rect {
    Math::Insets<CalcValue<PercentOr<Length>>> insets = {Percent(0), Percent(100), Percent(100), Percent(0)};
    Math::Radii<CalcValue<PercentOr<Length>>> borderRadius = {Percent(0)};

    void repr(Io::Emit& e) const {
        e("(rect {} {})", insets, borderRadius);
    }
};

export template <>
struct ValueParser<Rect> {
    static Res<Rect> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("rect(")) {
            Rect result;
            Cursor<Css::Sst> scan = c->content;

            eatWhitespace(scan);
            if (not parseValue<Keywords::Auto>(scan))
                result.insets.top = try$(parseValue<CalcValue<PercentOr<Length>>>(scan));

            eatWhitespace(scan);
            if (not parseValue<Keywords::Auto>(scan))
                result.insets.end = try$(parseValue<CalcValue<PercentOr<Length>>>(scan));

            eatWhitespace(scan);
            if (not parseValue<Keywords::Auto>(scan))
                result.insets.bottom = try$(parseValue<CalcValue<PercentOr<Length>>>(scan));

            eatWhitespace(scan);
            if (not parseValue<Keywords::Auto>(scan))
                result.insets.start = try$(parseValue<CalcValue<PercentOr<Length>>>(scan));

            eatWhitespace(scan);
            if (scan.skip(Css::Token::ident("round"))) {
                eatWhitespace(scan);
                if (auto radii = parseValue<Math::Radii<CalcValue<PercentOr<Length>>>>(scan)) {
                    result.borderRadius = radii.unwrap();
                }
                eatWhitespace(scan);
            }
            if (not scan.ended())
                return Error::invalidData("unexpected data in rect");

            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid rect");
    }
};

// MARK: Xywh ------------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-xywh

export struct Xywh {
    Math::Rect<CalcValue<PercentOr<Length>>> rect = {Percent(0), Percent(0), Percent(0), Percent(0)};
    Math::Radii<CalcValue<PercentOr<Length>>> borderRadius = {Percent(0)};

    void repr(Io::Emit& e) const {
        e("(xywh {} {})", rect, borderRadius);
    }
};

export template <>
struct ValueParser<Xywh> {
    static Res<Xywh> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("xywh(")) {
            Xywh result;
            Cursor<Css::Sst> scan = c->content;

            eatWhitespace(scan);
            result.rect.x = try$(parseValue<CalcValue<PercentOr<Length>>>(scan));

            eatWhitespace(scan);
            result.rect.y = try$(parseValue<CalcValue<PercentOr<Length>>>(scan));

            eatWhitespace(scan);
            result.rect.width = try$(parseValue<CalcValue<PercentOr<Length>>>(scan));

            eatWhitespace(scan);
            result.rect.height = try$(parseValue<CalcValue<PercentOr<Length>>>(scan));

            eatWhitespace(scan);
            if (scan.skip(Css::Token::ident("round"))) {
                eatWhitespace(scan);
                if (auto radii = parseValue<Math::Radii<CalcValue<PercentOr<Length>>>>(scan)) {
                    result.borderRadius = radii.unwrap();
                }
                eatWhitespace(scan);
            }

            if (not scan.ended())
                return Error::invalidData("unexpected data in xywh");

            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid xywh");
    }
};

// MARK: Basic Shape -----------------------------------------------------------
// https://drafts.fxtf.org/css-masking/#the-clip-path
//  [ <basic-shape> || <geometry-box> ]

export using BasicShapeFunction = Union<
    Circle, Ellipse, Inset, Path, Polygon, Rect, Xywh>;

export struct BasicShape {
    Opt<BasicShapeFunction> shape;
    GeometryBox referenceBox = Keywords::BORDER_BOX;

    void repr(Io::Emit& e) const {
        e("(basic-shape {} {})", shape, referenceBox);
    }
};

export template <>
struct ValueParser<BasicShape> {
    static Res<BasicShape> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        BasicShape resultShape;
        auto box = parseValue<GeometryBox>(c);
        auto shape = parseValue<BasicShapeFunction>(c);

        if (shape and not box)
            box = parseValue<GeometryBox>(c);

        if (not shape and not box)
            return Error::invalidData("expected basic shape or geometry box");

        resultShape.referenceBox = box.unwrapOr(Keywords::BORDER_BOX);

        if (shape)
            resultShape.shape = shape.unwrap();

        return Ok(resultShape);
    }
};

} // namespace Vaev
