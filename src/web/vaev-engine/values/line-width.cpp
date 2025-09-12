module;

#include <karm-math/au.h>

export module Vaev.Engine:values.lineWidth;

import :css;
import :values.base;
import :values.calc;
import :values.keywords;
import :values.length;

namespace Vaev {

// MARK: Line Width -------------------------------------------------------------
// https://drafts.csswg.org/css-backgrounds/#typedef-line-width

export constexpr Au THIN_VALUE = 1_au;
export constexpr Au MEDIUM_VALUE = 3_au;
export constexpr Au THICK_VALUE = 5_au;

export using LineWidth = Union<
    Keywords::Thin,
    Keywords::Medium,
    Keywords::Thick,
    CalcValue<Length>>;

export template <>
struct ValueParser<LineWidth> {
    static Res<LineWidth> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::ident("thin")) {
            c.next();
            return Ok(Keywords::THIN);
        }
        if (c.peek() == Css::Token::ident("medium")) {
            c.next();
            return Ok(Keywords::MEDIUM);
        }
        if (c.peek() == Css::Token::ident("thick")) {
            c.next();
            return Ok(Keywords::THICK);
        }

        return Ok(try$(parseValue<CalcValue<Length>>(c)));
    }
};

} // namespace Vaev
