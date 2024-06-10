#pragma once

#include "length.h"

namespace Web {

enum struct Clear {
    NONE,

    LEFT,
    RIGHT,
    BOTH,
    INLINE_START,
    INLINE_END,
};

enum struct Float {
    NONE,

    INLINE_START,
    INLINE_END,
    LEFT,
    RIGHT,
};

} // namespace Web
