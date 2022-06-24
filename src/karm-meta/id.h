#pragma once

#include <karm-base/std.h>

namespace Karm::Meta {

using Id = size_t;

template <typename T>
static Id makeId() {
    return reinterpret_cast<Id>(__PRETTY_FUNCTION__);
}

} // namespace Karm::Meta
