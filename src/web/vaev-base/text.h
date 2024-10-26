#pragma once

namespace Vaev {

// CSS Text Module Level 4
// https://drafts.csswg.org/css-text-4

// MARK: Text Transform --------------------------------------------------------
// https://drafts.csswg.org/css-text-4/#text-transform

enum struct TextTransform {
    NONE,

    UPPERCASE,
    LOWERCASE,

    _LEN,
};

struct Text {
    TextTransform transform;
};

} // namespace Vaev
