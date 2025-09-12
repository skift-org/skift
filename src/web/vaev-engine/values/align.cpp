export module Vaev.Engine:values.align;

import Karm.Core;
import :css;
import :values.base;

using namespace Karm;

namespace Vaev {

// MARK: Align -----------------------------------------------------------------

// https://drafts.csswg.org/css-align-3/#propdef-align-content
// https://drafts.csswg.org/css-align-3/#propdef-justify-content
// https://drafts.csswg.org/css-align-3/#propdef-justify-self
// https://drafts.csswg.org/css-align-3/#propdef-align-self
// https://drafts.csswg.org/css-align-3/#propdef-justify-items
// https://drafts.csswg.org/css-align-3/#propdef-align-items

export struct [[gnu::packed]] Align {
    enum Prefix : u8 {
        NO_PREFIX,

        LEGACY,
        SAFE,
        UNSAFE,

        // NOTE: This enum is pack with 2 bits, so the max value is 3

        _LEN0,
    };

    enum Keywords : u8 {
        NO_KEYWORD,

        AUTO,
        NORMAL,

        // Positional Alignment
        CENTER,
        START,
        END,
        SELF_START,
        SELF_END,
        FLEX_START,
        FLEX_END,
        LEFT,
        RIGHT,

        // BaseLine Alignment
        FIRST_BASELINE,
        LAST_BASELINE,

        // Distribution Alignment
        STRETCH,
        SPACE_BETWEEN,
        SPACE_AROUND,
        SPACE_EVENLY,

        // NOTE: This enum is pack with 6 bits, so the max value is 63

        _LEN1,
    };

    Prefix prefix : 2;
    Keywords keyword : 6;

    constexpr Align() : prefix(NO_PREFIX), keyword(NO_KEYWORD) {}

    constexpr Align(Prefix prefix) : prefix(prefix), keyword(NO_KEYWORD) {}

    constexpr Align(Keywords keyword) : prefix(NO_PREFIX), keyword(keyword) {}

    constexpr Align(Prefix prefix, Keywords keyword) : prefix(prefix), keyword(keyword) {}

    bool operator==(Prefix prefix) const { return this->prefix == prefix; }

    bool operator==(Keywords keyword) const { return this->keyword == keyword; }

    void repr(Io::Emit& e) const {
        e("{} {}", prefix, keyword);
    }
};

export template <>
struct ValueParser<Align> {
    static Res<Align> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        Align align;

        if (c.skip(Css::Token::ident("legacy"))) {
            align.prefix = Align::Prefix::LEGACY;
        } else if (c.skip(Css::Token::ident("safe"))) {
            align.prefix = Align::Prefix::SAFE;
        } else if (c.skip(Css::Token::ident("unsafe"))) {
            align.prefix = Align::Prefix::UNSAFE;
        }

        if (c.skip(Css::Token::ident("auto"))) {
            align.keyword = Align::Keywords::AUTO;
        } else if (c.skip(Css::Token::ident("normal"))) {
            align.keyword = Align::Keywords::NORMAL;
        } else if (c.skip(Css::Token::ident("center"))) {
            align.keyword = Align::Keywords::CENTER;
        } else if (c.skip(Css::Token::ident("start"))) {
            align.keyword = Align::Keywords::START;
        } else if (c.skip(Css::Token::ident("end"))) {
            align.keyword = Align::Keywords::END;
        } else if (c.skip(Css::Token::ident("self-start"))) {
            align.keyword = Align::Keywords::SELF_START;
        } else if (c.skip(Css::Token::ident("self-end"))) {
            align.keyword = Align::Keywords::SELF_END;
        } else if (c.skip(Css::Token::ident("flex-start"))) {
            align.keyword = Align::Keywords::FLEX_START;
        } else if (c.skip(Css::Token::ident("flex-end"))) {
            align.keyword = Align::Keywords::FLEX_END;
        } else if (c.skip(Css::Token::ident("left"))) {
            align.keyword = Align::Keywords::LEFT;
        } else if (c.skip(Css::Token::ident("right"))) {
            align.keyword = Align::Keywords::RIGHT;
        } else if (c.skip(Css::Token::ident("first-baseline"))) {
            align.keyword = Align::Keywords::FIRST_BASELINE;
        } else if (c.skip(Css::Token::ident("last-baseline"))) {
            align.keyword = Align::Keywords::LAST_BASELINE;
        } else if (c.skip(Css::Token::ident("stretch"))) {
            align.keyword = Align::Keywords::STRETCH;
        } else if (c.skip(Css::Token::ident("space-between"))) {
            align.keyword = Align::Keywords::SPACE_BETWEEN;
        } else if (c.skip(Css::Token::ident("space-around"))) {
            align.keyword = Align::Keywords::SPACE_AROUND;
        } else if (c.skip(Css::Token::ident("space-evenly"))) {
            align.keyword = Align::Keywords::SPACE_EVENLY;
        }

        if (align == Align::NO_PREFIX and align == Align::NO_KEYWORD)
            return Error::invalidData("expected align value");

        return Ok(align);
    }
};

export struct AlignProps {
    // https://drafts.csswg.org/css-align-3/#propdef-align-content
    Align alignContent;

    // https://drafts.csswg.org/css-align-3/#propdef-justify-content
    Align justifyContent;

    // https://drafts.csswg.org/css-align-3/#propdef-justify-self
    Align justifySelf;

    // https://drafts.csswg.org/css-align-3/#propdef-align-self
    Align alignSelf;

    // https://drafts.csswg.org/css-align-3/#propdef-justify-items
    Align justifyItems;

    // https://drafts.csswg.org/css-align-3/#propdef-align-items
    Align alignItems;

    void repr(Io::Emit& e) const {
        e("(aligns");
        e(" alignContent={}", alignContent);
        e(" justifyContent={}", justifyContent);
        e(" justifySelf={}", justifySelf);
        e(" alignSelf={}", alignSelf);
        e(" justifyItems={}", justifyItems);
        e(" alignItems={}", alignItems);
        e(")");
    }
};

} // namespace Vaev
