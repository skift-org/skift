#pragma once

#include <karm-base/string.h>

namespace Karm::Sys {

struct Path {
    Str _str;

    Path() = default;

    Path(char const *str) : _str(str) {}

    Path(Str path) : _str(path) {}

    Ordr cmp(Path const &other) const {
        return ::cmp(_str, other._str);
    }

    Str str() const {
        return _str;
    }
};

} // namespace Karm::Sys
