#pragma once

#include <karm-base/string.h>

namespace Karm::Sys {

struct Path {
    Str _str;

    Path() = default;

    Path(Str path) : _str(path) {}

    Ordr cmp(Path const &other) const {
        return _str.cmp(other._str);
    }

    Str str() const {
        return _str;
    }
};

} // namespace Karm::Sys
