#pragma once

#include <karm-io/emit.h>

namespace Vaev {

// CSS Text Module Level 4
// https://drafts.csswg.org/css-text-4

// MARK: Text Alignment --------------------------------------------------------
// https://drafts.csswg.org/css-text/#text-align-property

enum struct TextAlign {
    START,
    END,
    LEFT,
    RIGHT,
    CENTER,
    JUSTIFY,
    MATCH_PARENT,
    JUSTIFY_ALL,

    _LEN,
};

// MARK: Text Transform --------------------------------------------------------
// https://drafts.csswg.org/css-text-4/#text-transform

enum struct TextTransform {
    NONE,

    UPPERCASE,
    LOWERCASE,

    _LEN,
};

// MARK: White Space -----------------------------------------------------------
// https://drafts.csswg.org/css-text/#white-space-property

enum struct WhiteSpace {
    NORMAL,
    PRE,
    NOWRAP,
    PRE_WRAP,
    BREAK_SPACES,
    PRE_LINE,

    _LEN,
};

struct TextProps {
    TextAlign align = TextAlign::START;
    TextTransform transform;
    WhiteSpace whiteSpace = WhiteSpace::NORMAL;

    void repr(Io::Emit &e) const {
        e("(text");
        e(" align: {}", align);
        e(" transform: {}", transform);
        e(" whiteSpace: {}", whiteSpace);
        e(")");
    }
};

} // namespace Vaev
