#pragma once

#include "width.h"

namespace Vaev {

// https://www.w3.org/TR/CSS22/visuren.html#propdef-position
enum struct Position {
    STATIC,

    RELATIVE,
    ABSOLUTE,
    FIXED,
    STICKY,

    _LEN,
};

using Margin = Math::Insets<Width>;

using Padding = Math::Insets<CalcValue<PercentOr<Length>>>;

// https://www.w3.org/TR/CSS22/visuren.html#propdef-top
// https://www.w3.org/TR/CSS22/visuren.html#propdef-right
// https://www.w3.org/TR/CSS22/visuren.html#propdef-bottom
// https://www.w3.org/TR/CSS22/visuren.html#propdef-left
using Offsets = Math::Insets<Width>;

using Gap = Union<Keywords::Normal, CalcValue<PercentOr<Length>>>;

struct Gaps {
    Gap x = Keywords::NORMAL;
    Gap y = Keywords::NORMAL;

    void repr(Io::Emit& e) const {
        e("(gaps {} {})", x, y);
    }
};

} // namespace Vaev
