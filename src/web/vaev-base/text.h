#pragma once

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

struct TextProps {
    TextAlign align = TextAlign::START;
    TextTransform transform;
};

} // namespace Vaev
