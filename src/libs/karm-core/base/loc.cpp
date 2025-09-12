export module Karm.Core:base.loc;

import :base.string;

namespace Karm {

export struct Loc {
    Str file{};
    Str func{};
    usize line{};
    usize column{};

    static constexpr Loc current(
        Str file = __builtin_FILE(),
        Str func = __builtin_FUNCTION(),
        usize line = __builtin_LINE(),
        usize column = __builtin_COLUMN()
    ) {
        return {file, func, line, column};
    }
};

} // namespace Karm
