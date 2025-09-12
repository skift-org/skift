module;

#include <karm-gfx/borders.h>
#include <karm-gfx/fill.h>
#include <karm-math/au.h>
#include <karm-math/radii.h>

export module Vaev.Engine:values.borders;

import :css;
import :values.base;
import :values.calc;
import :values.color;
import :values.length;
import :values.lineWidth;
import :values.percent;

namespace Vaev {

// MARK: Border Style ----------------------------------------------------------
// https://www.w3.org/TR/CSS22/box.html#border-style-properties

export template <>
struct ValueParser<Gfx::BorderStyle> {
    static Res<Gfx::BorderStyle> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of property");

        if (c.skip(Css::Token::ident("none"))) {
            return Ok(Gfx::BorderStyle::NONE);
        } else if (c.skip(Css::Token::ident("solid"))) {
            return Ok(Gfx::BorderStyle::SOLID);
        } else if (c.skip(Css::Token::ident("dashed"))) {
            return Ok(Gfx::BorderStyle::DASHED);
        } else if (c.skip(Css::Token::ident("dotted"))) {
            return Ok(Gfx::BorderStyle::DOTTED);
        } else if (c.skip(Css::Token::ident("hidden"))) {
            return Ok(Gfx::BorderStyle::HIDDEN);
        } else if (c.skip(Css::Token::ident("double"))) {
            return Ok(Gfx::BorderStyle::DOUBLE);
        } else if (c.skip(Css::Token::ident("groove"))) {
            return Ok(Gfx::BorderStyle::GROOVE);
        } else if (c.skip(Css::Token::ident("ridge"))) {
            return Ok(Gfx::BorderStyle::RIDGE);
        } else if (c.skip(Css::Token::ident("outset"))) {
            return Ok(Gfx::BorderStyle::OUTSET);
        } else {
            return Error::invalidData("unknown border-style");
        }
    }
};

// MARK: Border ----------------------------------------------------------------

export struct Border {
    LineWidth width = Keywords::MEDIUM;
    Gfx::BorderStyle style;
    Color color = Keywords::CURRENT_COLOR;

    void repr(Io::Emit& e) const {
        e("(border {} {} {})", width, style, color);
    }
};

export template <>
struct ValueParser<Border> {
    static Res<Border> parse(Cursor<Css::Sst>& c) {
        Border border;
        while (not c.ended()) {
            eatWhitespace(c);

            auto width = parseValue<LineWidth>(c);
            if (width) {
                border.width = width.unwrap();
                continue;
            }

            auto color = parseValue<Color>(c);
            if (color) {
                border.color = color.unwrap();
                continue;
            }

            auto style = parseValue<Gfx::BorderStyle>(c);
            if (style) {
                border.style = style.unwrap();
                continue;
            }

            break;
        }

        return Ok(border);
    }
};

export struct BorderProps {
    Border top, start, bottom, end;
    Math::Radii<CalcValue<PercentOr<Length>>> radii = {Length(0_au)};

    void all(Border b) {
        top = start = bottom = end = b;
    }

    void repr(Io::Emit& e) const {
        e("(borders");
        e(" top={}", top);
        e(" start={}", start);
        e(" bottom={}", bottom);
        e(" end={}", end);
        e(" radii={}", radii);
        e(")");
    }
};

export template <typename T>
struct ValueParser<Math::Radii<T>> {
    static Res<Math::Radii<T>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto value1 = parseValue<PercentOr<Length>>(c);
        if (not value1)
            return Ok(parsePostSlash(c, Math::Radii<T>{Length{}}));

        auto value2 = parseValue<PercentOr<Length>>(c);
        if (not value2)
            return Ok(parsePostSlash(c, Math::Radii<T>{value1.take()}));

        auto value3 = parseValue<PercentOr<Length>>(c);
        if (not value3)
            return Ok(parsePostSlash(c, Math::Radii<T>{value1.take(), value2.take()}));

        auto value4 = parseValue<PercentOr<Length>>(c);
        if (not value4)
            return Ok(parsePostSlash(c, Math::Radii<T>{value1.take(), value2.take(), value3.take(), value2.take()}));

        return Ok(parsePostSlash(c, Math::Radii<T>{value1.take(), value2.take(), value3.take(), value4.take()}));
    }

    static Math::Radii<T> parsePostSlash(Cursor<Css::Sst>& c, Math::Radii<T> radii) {
        // if parse a /
        // 1 value-- > border all(a, d, e, h)
        // 2 values-- > 1 = top - start + bottom - end 2 = the others
        // 3 values-- > 1 = top - start, 2 = top - end + bottom - start, 3 = bottom - end
        // 4 values-- > 1 = top - start, 2 = top - end 3 = bottom - end, 4 = bottom - start
        eatWhitespace(c);
        if (not c.ended() and c.peek().token.data == "/"s) {
            c.next();
            eatWhitespace(c);
            auto value1 = parseValue<PercentOr<Length>>(c);
            if (not value1) {
                return radii;
            }

            auto value2 = parseValue<PercentOr<Length>>(c);
            if (not value2) {
                radii.a = value1.take();
                radii.d = value1.take();
                radii.e = value1.take();
                radii.h = value1.take();
                return radii;
            }

            eatWhitespace(c);
            auto value3 = parseValue<PercentOr<Length>>(c);
            if (not value3) {
                radii.a = value1.take();
                radii.d = value2.take();
                radii.e = value1.take();
                radii.h = value2.take();
                return radii;
            }

            eatWhitespace(c);
            auto value4 = parseValue<PercentOr<Length>>(c);
            if (not value4) {
                radii.a = value1.take();
                radii.d = value2.take();
                radii.e = value3.take();
                radii.h = value2.take();

                return radii;
            }

            radii.a = value1.take();
            radii.d = value2.take();
            radii.e = value3.take();
            radii.h = value4.take();
            return radii;
        }

        return radii;
    }
};

} // namespace Vaev
