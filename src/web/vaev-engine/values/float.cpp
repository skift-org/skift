export module Vaev.Engine:values.float_;

import Karm.Core;

import :css;
import :values.base;
import :values.length;

using namespace Karm;

namespace Vaev {

// MARK: Clear & Float ---------------------------------------------------------

export enum struct Float {
    NONE,

    INLINE_START,
    INLINE_END,
    LEFT,
    RIGHT,

    _LEN
};

export template <>
struct ValueParser<Float> {
    static Res<Float> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none"))) {
            return Ok(Float::NONE);
        } else if (c.skip(Css::Token::ident("inline-start"))) {
            return Ok(Float::INLINE_START);
        } else if (c.skip(Css::Token::ident("inline-end"))) {
            return Ok(Float::INLINE_END);
        } else if (c.skip(Css::Token::ident("left"))) {
            return Ok(Float::LEFT);
        } else if (c.skip(Css::Token::ident("right"))) {
            return Ok(Float::RIGHT);
        }

        return Error::invalidData("expected float");
    }
};

export enum struct Clear {
    NONE,

    LEFT,
    RIGHT,
    BOTH,
    INLINE_START,
    INLINE_END,

    _LEN,
};

export template <>
struct ValueParser<Clear> {
    static Res<Clear> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none"))) {
            return Ok(Clear::NONE);
        } else if (c.skip(Css::Token::ident("left"))) {
            return Ok(Clear::LEFT);
        } else if (c.skip(Css::Token::ident("right"))) {
            return Ok(Clear::RIGHT);
        } else if (c.skip(Css::Token::ident("both"))) {
            return Ok(Clear::BOTH);
        } else if (c.skip(Css::Token::ident("inline-start"))) {
            return Ok(Clear::INLINE_START);
        } else if (c.skip(Css::Token::ident("inline-end"))) {
            return Ok(Clear::INLINE_END);
        }

        return Error::invalidData("expected clear");
    }
};

} // namespace Vaev
