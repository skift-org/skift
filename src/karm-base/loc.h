#pragma once

#include "string.h"

namespace Karm {

struct Loc {
    Str file{};
    Str function{};
    size_t line{};
    size_t column{};

    static Loc current(
        Str file = __builtin_FILE(),
        Str function = __builtin_FUNCTION(),
        size_t line = __builtin_LINE(),
        size_t column = __builtin_COLUMN()) {
        return {file, function, line, column};
    }
};

} // namespace Karm
