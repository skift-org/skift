module;

#include <karm-math/insets.h>

export module Vaev.Engine:values.insets;

import Karm.Core;

import :css;
import :values.base;
import :values.width;

using namespace Karm;

namespace Vaev {

// MARK: Position --------------------------------------------------------------
// https://www.w3.org/TR/CSS22/visuren.html#propdef-position
export enum struct Position {
    STATIC,

    RELATIVE,
    ABSOLUTE,
    FIXED,
    STICKY,

    _LEN,
};

export bool impliesRemovingFromFlow(Position position) {
    return position == Position::ABSOLUTE || position == Position::FIXED;
}

export template <>
struct ValueParser<Position> {
    static Res<Position> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("static")))
            return Ok(Position::STATIC);
        else if (c.skip(Css::Token::ident("relative")))
            return Ok(Position::RELATIVE);
        else if (c.skip(Css::Token::ident("absolute")))
            return Ok(Position::ABSOLUTE);
        else if (c.skip(Css::Token::ident("fixed")))
            return Ok(Position::FIXED);
        else if (c.skip(Css::Token::ident("sticky")))
            return Ok(Position::STICKY);
        else
            return Error::invalidData("expected position");
    }
};

export using Margin = Math::Insets<Width>;

export using Padding = Math::Insets<CalcValue<PercentOr<Length>>>;

// https://www.w3.org/TR/CSS22/visuren.html#propdef-top
// https://www.w3.org/TR/CSS22/visuren.html#propdef-right
// https://www.w3.org/TR/CSS22/visuren.html#propdef-bottom
// https://www.w3.org/TR/CSS22/visuren.html#propdef-left
export using Offsets = Math::Insets<Width>;

export template <typename T>
struct ValueParser<Math::Insets<T>> {
    static Res<Math::Insets<T>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto top = parseValue<T>(c);
        if (not top)
            return top.none();

        auto right = parseValue<T>(c);
        if (not right)
            return Ok(Math::Insets<T>{top.take()});

        auto bottom = parseValue<T>(c);
        if (not bottom)
            return Ok(Math::Insets<T>{top.take(), right.take()});

        auto left = parseValue<T>(c);
        if (not left)
            return Ok(Math::Insets<T>{top.take(), right.take(), bottom.take()});

        return Ok(Math::Insets<T>{top.take(), right.take(), bottom.take(), left.take()});
    }
};

export using Gap = Union<
    Keywords::Normal,
    CalcValue<PercentOr<Length>>>;

export struct Gaps {
    Gap x = Keywords::NORMAL;
    Gap y = Keywords::NORMAL;

    void repr(Io::Emit& e) const {
        e("(gaps {} {})", x, y);
    }
};

} // namespace Vaev
