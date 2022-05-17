#pragma once

#include <karm-base/string.h>

namespace Karm::Sys {

struct Path {
    Str _buf;

    Path() = default;

    Path(Str path) : _buf(path) {}

    Ordr cmp(Path const &other) const {
        return _buf.cmp(other._buf);
    }
};

} // namespace Karm::Sys
