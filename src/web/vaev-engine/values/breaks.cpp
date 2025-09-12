export module Vaev.Engine:values.breaks;

import Karm.Core;

import :css;
import :values.base;
import :values.primitives;

using namespace Karm;

namespace Vaev {

// 3.1. Breaks Between Boxes: the break-before and break-after properties
// https://www.w3.org/TR/css-break-3/#break-between
// https://www.w3.org/TR/css-break-3/#propdef-break-after
// https://www.w3.org/TR/css-break-3/#propdef-break-before

export enum struct BreakBetween {
    AUTO,
    AVOID,
    AVOID_PAGE,
    PAGE,
    LEFT,
    RIGHT,
    RECTO,
    VERSO,
    AVOID_COLUMN,
    COLUMN,
    AVOID_REGION,
    REGION,

    _LEN,
};

export template <>
struct ValueParser<BreakBetween> {
    static Res<BreakBetween> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("auto"))) {
            return Ok(BreakBetween::AUTO);
        } else if (c.skip(Css::Token::ident("avoid"))) {
            return Ok(BreakBetween::AVOID);
        } else if (c.skip(Css::Token::ident("avoid-page"))) {
            return Ok(BreakBetween::AVOID_PAGE);
        } else if (c.skip(Css::Token::ident("page"))) {
            return Ok(BreakBetween::PAGE);
        } else if (c.skip(Css::Token::ident("left"))) {
            return Ok(BreakBetween::LEFT);
        } else if (c.skip(Css::Token::ident("right"))) {
            return Ok(BreakBetween::RIGHT);
        } else if (c.skip(Css::Token::ident("recto"))) {
            return Ok(BreakBetween::RECTO);
        } else if (c.skip(Css::Token::ident("verso"))) {
            return Ok(BreakBetween::VERSO);
        } else if (c.skip(Css::Token::ident("avoid-column"))) {
            return Ok(BreakBetween::AVOID_COLUMN);
        } else if (c.skip(Css::Token::ident("column"))) {
            return Ok(BreakBetween::COLUMN);
        } else if (c.skip(Css::Token::ident("avoid-region"))) {
            return Ok(BreakBetween::AVOID_REGION);
        } else if (c.skip(Css::Token::ident("region"))) {
            return Ok(BreakBetween::REGION);
        }

        return Error::invalidData("expected break between value");
    }
};

// 3.2. Breaks Within Boxes: the break-inside property
// https://www.w3.org/TR/css-break-3/#break-within

export enum struct BreakInside {
    AUTO,
    AVOID,
    AVOID_PAGE,
    AVOID_COLUMN,
    AVOID_REGION,

    _LEN,
};

export template <>
struct ValueParser<BreakInside> {
    static Res<BreakInside> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("auto"))) {
            return Ok(BreakInside::AUTO);
        } else if (c.skip(Css::Token::ident("avoid"))) {
            return Ok(BreakInside::AVOID);
        } else if (c.skip(Css::Token::ident("avoid-page"))) {
            return Ok(BreakInside::AVOID_PAGE);
        } else if (c.skip(Css::Token::ident("avoid-column"))) {
            return Ok(BreakInside::AVOID_COLUMN);
        } else if (c.skip(Css::Token::ident("avoid-region"))) {
            return Ok(BreakInside::AVOID_REGION);
        }

        return Error::invalidData("expected break between value");
    }
};

// https://www.w3.org/TR/css-break-3
export struct BreakProps {
    BreakBetween before = BreakBetween::AUTO;
    BreakBetween after = BreakBetween::AUTO;
    BreakInside inside = BreakInside::AUTO;

    // Breaks Between Lines: orphans, widows
    // https://www.w3.org/TR/css-break-3/#widows-orphans
    Integer orphans = 2;
    Integer widows = 2;

    void repr(Io::Emit& e) const {
        e("(breaks");
        e(" before={}", before);
        e(" after={}", after);
        e(" inside={}", inside);
        e(" orphans={}", orphans);
        e(" widows={}", widows);
        e(")");
    }
};

} // namespace Vaev
