#pragma once

#include "length.h"

namespace Vaev {

enum struct Float {
    NONE,

    INLINE_START,
    INLINE_END,
    LEFT,
    RIGHT,

    _LEN
};

enum struct Clear {
    NONE,

    LEFT,
    RIGHT,
    BOTH,
    INLINE_START,
    INLINE_END,

    _LEN,
};

} // namespace Vaev
