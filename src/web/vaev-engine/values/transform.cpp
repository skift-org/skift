module;

#include <karm-logger/logger.h>
#include <karm-math/au.h>

export module Vaev.Engine:values.transform;

import :css;
import :values.base;
import :values.angle;
import :values.calc;
import :values.keywords;
import :values.percent;
import :values.primitives;

namespace Vaev {

// https://drafts.csswg.org/css-transforms
// TODO: Add support for 3d transforms and perspective.

// MARK: Transform Origin ------------------------------------------------------
// https://drafts.csswg.org/css-transforms/#transform-origin-property

export struct TransformOrigin {
    using OneValue = Union<Keywords::Left, Keywords::Center, Keywords::Right, CalcValue<PercentOr<Length>>>;

    using XOffsetKeyword = Union<Keywords::Left, Keywords::Center, Keywords::Right>;
    using XOffset = FlatUnion<XOffsetKeyword, CalcValue<PercentOr<Length>>>;
    using YOffsetKeyword = Union<Keywords::Top, Keywords::Center, Keywords::Bottom>;
    using YOffset = FlatUnion<YOffsetKeyword, CalcValue<PercentOr<Length>>>;

    XOffset xOffset;
    YOffset yOffset;

    void repr(Io::Emit& e) const {
        e("{} {}", xOffset, yOffset);
    }
};

export template <>
struct ValueParser<TransformOrigin> {
    static Res<TransformOrigin> _twoValues(Cursor<Css::Sst>& c) {
        auto xoffset = try$(parseValue<TransformOrigin::XOffset>(c));
        if (c.ended())
            return Error::invalidData("unexpected end of input after x-offset");

        auto yoffset = try$(parseValue<TransformOrigin::YOffset>(c));

        return Ok(TransformOrigin{
            .xOffset = std::move(xoffset),
            .yOffset = std::move(yoffset),
        });
    }

    static Res<TransformOrigin> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        {
            auto rollback = c.rollbackPoint();
            auto maybeTwoValues = _twoValues(c);
            if (maybeTwoValues) {
                rollback.disarm();
                return maybeTwoValues;
            }
        }

        auto singleValue = try$(parseValue<TransformOrigin::OneValue>(c));
        if (c.ended())
            return Error::invalidData("unexpected end of input after single value");

        return singleValue.visit(Visitor{
            [&](Keywords::Center) -> Res<TransformOrigin> {
                return Ok(TransformOrigin{
                    .xOffset = Keywords::CENTER,
                    .yOffset = Keywords::CENTER,
                });
            },

            [&](Keywords::Left) -> Res<TransformOrigin> {
                return Ok(TransformOrigin{
                    .xOffset = Keywords::LEFT,
                    .yOffset = Keywords::CENTER, // default y-offset
                });
            },
            [&](Keywords::Right) -> Res<TransformOrigin> {
                return Ok(TransformOrigin{
                    .xOffset = Keywords::RIGHT,
                    .yOffset = Keywords::CENTER, // default y-offset
                });
            },

            [&](Keywords::Top) -> Res<TransformOrigin> {
                return Ok(TransformOrigin{
                    .xOffset = Keywords::CENTER, // default x-offset
                    .yOffset = Keywords::TOP,
                });
            },
            [&](Keywords::Bottom) -> Res<TransformOrigin> {
                return Ok(TransformOrigin{
                    .xOffset = Keywords::CENTER, // default x-offset
                    .yOffset = Keywords::BOTTOM,
                });
            },

            [&](CalcValue<PercentOr<Length>> value) -> Res<TransformOrigin> {
                return Ok(TransformOrigin{
                    .xOffset = std::move(value),
                    .yOffset = Keywords::CENTER, // default y-offset
                });
            },
        });
    }
};

// MARK: Transform Box ---------------------------------------------------------
// https://drafts.csswg.org/css-transforms/#transform-box

export using TransformBox = Union<
    Keywords::ContentBox,
    Keywords::BorderBox,
    Keywords::FillBox,
    Keywords::StrokeBox,
    Keywords::ViewBox>;

// MARK: Transform Functions ---------------------------------------------------
// https://drafts.csswg.org/css-transforms/#transform-functions

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-matrix
export struct MatrixTransform {
    Array<CalcValue<Number>, 6> values;

    void repr(Io::Emit& e) const {
        e("matrix({}, {}, {}, {}, {}, {})", values[0], values[1], values[2], values[3], values[4], values[5]);
    }
};

export template <>
struct ValueParser<MatrixTransform> {
    static Res<MatrixTransform> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix != Css::Token::function("matrix("))
            return Error::invalidData("expected matrix function");

        Cursor<Css::Sst> content = c->content;
        eatWhitespace(content);
        Array<CalcValue<Number>, 6> values;

        for (usize i = 0; i < 6; ++i) {
            if (content.ended())
                return Error::invalidData("unexpected end of input");

            auto value = try$(parseValue<CalcValue<Number>>(content));
            values[i] = std::move(value);
            if (i < 5)
                skipOmmitableComma(content);
        }

        c.next(); // consume the function token
        return Ok(MatrixTransform{std::move(values)});
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-translate
export struct TranslateTransform {
    CalcValue<PercentOr<Length>> x;
    CalcValue<PercentOr<Length>> y;

    void repr(Io::Emit& e) const {
        e("translate({}, {})", x, y);
    }
};

export template <>
struct ValueParser<TranslateTransform> {
    static Res<TranslateTransform> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("translateX(")) {
            Cursor<Css::Sst> content = c->content;
            eatWhitespace(content);
            auto tx = try$(parseValue<CalcValue<PercentOr<Length>>>(content));
            eatWhitespace(content);
            if (not content.ended()) {
                return Error::invalidData("unexpected content after translateX function");
            }
            c.next(); // consume the function token
            return Ok(TranslateTransform{std::move(tx), Length{0_au}});
        }

        if (c->prefix == Css::Token::function("translateY(")) {
            Cursor<Css::Sst> content = c->content;
            eatWhitespace(content);
            auto ty = try$(parseValue<CalcValue<PercentOr<Length>>>(content));
            eatWhitespace(content);
            if (not content.ended())
                return Error::invalidData("unexpected content after translateY function");
            c.next(); // consume the function token
            return Ok(TranslateTransform{Length{0_au}, std::move(ty)});
        }

        if (c->prefix != Css::Token::function("translate("))
            return Error::invalidData("expected translate function");

        Cursor<Css::Sst> content = c->content;
        eatWhitespace(content);

        auto tx = try$(parseValue<CalcValue<PercentOr<Length>>>(content));
        skipOmmitableComma(content);

        if (content.ended()) {
            // f <ty> is not provided, ty has zero as a value.
            c.next(); // consume the function token
            return Ok(TranslateTransform{std::move(tx), Length{0_au}});
        }

        auto ty = try$(parseValue<CalcValue<PercentOr<Length>>>(content));
        eatWhitespace(content);
        if (not content.ended())
            return Error::invalidData("unexpected content after scale function");

        c.next(); // consume the function token
        return Ok(TranslateTransform{std::move(tx), std::move(ty)});
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-scale
export struct ScaleTransform {
    CalcValue<Number> x;
    CalcValue<Number> y;

    void repr(Io::Emit& e) const {
        e("scale({}, {})", x, y);
    }
};

export template <>
struct ValueParser<ScaleTransform> {
    static Res<ScaleTransform> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("scaleX(")) {
            Cursor<Css::Sst> content = c->content;
            eatWhitespace(content);
            auto sx = try$(parseValue<CalcValue<Number>>(content));
            eatWhitespace(content);
            if (not content.ended()) {
                return Error::invalidData("unexpected content after scaleX function");
            }
            c.next(); // consume the function token
            return Ok(ScaleTransform{std::move(sx), Number{1}});
        }

        if (c->prefix == Css::Token::function("scaleY(")) {
            Cursor<Css::Sst> content = c->content;
            eatWhitespace(content);
            auto sy = try$(parseValue<CalcValue<Number>>(content));
            eatWhitespace(content);
            if (not content.ended())
                return Error::invalidData("unexpected content after scaleY function");
            c.next(); // consume the function token
            return Ok(ScaleTransform{Number{1}, std::move(sy)});
        }

        if (c->prefix != Css::Token::function("scale("))
            return Error::invalidData("expected scale function");

        Cursor<Css::Sst> content = c->content;
        eatWhitespace(content);

        auto x = try$(parseValue<CalcValue<Number>>(content));
        skipOmmitableComma(content);

        // If the second parameter is not provided, it takes a value equal to the first.
        if (content.ended()) {
            auto y = x;
            c.next(); // consume the function token
            return Ok(ScaleTransform{std::move(x), std::move(y)});
        }

        auto y = try$(parseValue<CalcValue<Number>>(content));
        eatWhitespace(content);
        if (not content.ended())
            return Error::invalidData("unexpected content after scale function");

        c.next(); // consume the function token
        return Ok(ScaleTransform{std::move(x), std::move(y)});
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-rotate
export struct RotateTransform {
    CalcValue<Angle> value;

    void repr(Io::Emit& e) const {
        e("rotate({})", value);
    }
};

export template <>
struct ValueParser<RotateTransform> {
    static Res<RotateTransform> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix != Css::Token::function("rotate("))
            return Error::invalidData("expected rotate function");

        Cursor<Css::Sst> content = c->content;
        eatWhitespace(content);

        auto value = try$(parseValue<CalcValue<Angle>>(content));
        eatWhitespace(content);

        if (not content.ended())
            return Error::invalidData("unexpected content after rotate function");

        c.next(); // consume the function token
        return Ok(RotateTransform{std::move(value)});
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-skew
export struct SkewTransform {
    CalcValue<Angle> x;
    CalcValue<Angle> y;

    void repr(Io::Emit& e) const {
        e("skew({}, {})", x, y);
    }
};

export template <>
struct ValueParser<SkewTransform> {
    static Res<SkewTransform> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix != Css::Token::function("skew("))
            return Error::invalidData("expected skew function");

        Cursor<Css::Sst> content = c->content;
        eatWhitespace(content);

        auto x = try$(parseValue<CalcValue<Angle>>(content));
        skipOmmitableComma(content);

        // If the second parameter is not provided, it has a zero value.
        if (content.ended()) {
            c.next(); // consume the function token
            return Ok(SkewTransform{std::move(x), Angle::fromDegree(0.)});
        }

        auto y = try$(parseValue<CalcValue<Angle>>(content));
        eatWhitespace(content);

        if (not content.ended())
            return Error::invalidData("unexpected content after skew function");

        c.next(); // consume the function token
        return Ok(SkewTransform{std::move(x), std::move(y)});
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-skewx
export struct SkewXTransform {
    CalcValue<Angle> value;

    void repr(Io::Emit& e) const {
        e("skewX({})", value);
    }
};

export template <>
struct ValueParser<SkewXTransform> {
    static Res<SkewXTransform> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix != Css::Token::function("skewX("))
            return Error::invalidData("expected skewX function");

        Cursor<Css::Sst> content = c->content;
        eatWhitespace(content);

        auto value = try$(parseValue<CalcValue<Angle>>(content));
        eatWhitespace(content);

        if (not content.ended())
            return Error::invalidData("unexpected content after skewX function");

        c.next(); // consume the function token
        return Ok(SkewXTransform{std::move(value)});
    }
};

// https://www.w3.org/TR/css-transforms-1/#funcdef-transform-skewy
export struct SkewYTransform {
    CalcValue<Angle> value;

    void repr(Io::Emit& e) const {
        e("skewY({})", value);
    }
};

export template <>
struct ValueParser<SkewYTransform> {
    static Res<SkewYTransform> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix != Css::Token::function("skewY("))
            return Error::invalidData("expected skewY function");

        Cursor<Css::Sst> content = c->content;
        eatWhitespace(content);

        auto value = try$(parseValue<CalcValue<Angle>>(content));
        eatWhitespace(content);

        if (not content.ended())
            return Error::invalidData("unexpected content after skewY function");

        c.next(); // consume the function token
        return Ok(SkewYTransform{std::move(value)});
    }
};

export using TransformFunction = Union<
    MatrixTransform,
    TranslateTransform,
    ScaleTransform,
    RotateTransform,
    SkewTransform,
    SkewXTransform,
    SkewYTransform>;

// MARK: Transform -------------------------------------------------------------
// https://drafts.csswg.org/css-transforms/#propdef-transform

export using Transform = Union<Keywords::None, Vec<TransformFunction>>;

export template <>
struct ValueParser<Transform> {
    static Res<Transform> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none"))) {
            return Ok(Keywords::NONE);
        }

        Vec<TransformFunction> functions;
        while (not c.ended() and c->type == Css::Sst::FUNC) {
            auto function = try$(parseValue<TransformFunction>(c));
            functions.pushBack(std::move(function));
            eatWhitespace(c);
        }

        return Ok(std::move(functions));
    }
};

} // namespace Vaev
