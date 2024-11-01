#pragma once

#include <karm-base/base.h>
#include <karm-io/emit.h>

namespace Vaev::Style {

struct [[gnu::packed]] Align {
    enum Perfix : u8 {
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

    Perfix perfix : 2;
    Keywords keyword : 6;

    constexpr Align() : perfix(NO_PREFIX), keyword(NO_KEYWORD) {}

    constexpr Align(Perfix perfix) : perfix(perfix), keyword(NO_KEYWORD) {}

    constexpr Align(Keywords keyword) : perfix(NO_PREFIX), keyword(keyword) {}

    constexpr Align(Perfix perfix, Keywords keyword) : perfix(perfix), keyword(keyword) {}

    bool operator==(Perfix perfix) const { return this->perfix == perfix; }

    bool operator==(Keywords keyword) const { return this->keyword == keyword; }

    void repr(Io::Emit &e) const {
        e("{} {}", perfix, keyword);
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
};

} // namespace Vaev::Style
