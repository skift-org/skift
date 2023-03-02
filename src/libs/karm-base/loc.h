#pragma once

#include "string.h"

namespace Karm {

struct Loc {
    Str file{};
    Str function{};
    usize line{};
    usize column{};

    static constexpr Loc current(
        Str file = __builtin_FILE(),
        Str function = __builtin_FUNCTION(),
        usize line = __builtin_LINE(),
        usize column = __builtin_COLUMN()) {
        return {file, function, line, column};
    }
};

} // namespace Karm
