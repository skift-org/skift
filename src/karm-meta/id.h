#pragma once

#include <karm-base/panic.h>
#include <karm-base/std.h>

namespace Karm::Meta {

using Id = size_t;

template <typename T>
static Id makeId() {
    return reinterpret_cast<Id>(__PRETTY_FUNCTION__ + 41);
}

template <typename T>
static char *makeName() {
    return __PRETTY_FUNCTION__ + 40;
}

} // namespace Karm::Meta
