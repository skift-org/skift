#pragma once

#include <karm-base/base.h>
#include <karm-io/emit.h>

namespace Vaev::Style {

struct [[gnu::packed]] Align {
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

    void repr(Io::Emit &e) const {
        e("{} {}", prefix, keyword);
    }
};

struct AlignProps {
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

    void repr(Io::Emit &e) const {
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

} // namespace Vaev::Style
