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

using Margin = Math::Spacing<Width>;

using Padding = Math::Spacing<PercentOr<Length>>;

// https://www.w3.org/TR/CSS22/visuren.html#propdef-top
// https://www.w3.org/TR/CSS22/visuren.html#propdef-right
// https://www.w3.org/TR/CSS22/visuren.html#propdef-bottom
// https://www.w3.org/TR/CSS22/visuren.html#propdef-left
using Offsets = Math::Spacing<Width>;

} // namespace Vaev
