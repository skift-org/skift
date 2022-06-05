#pragma once

#include <karm-base/std.h>

namespace Karm::Meta {

using Id = uintptr_t;

template <typename T>
static inline Id makeId() {
    static uint32_t _;
    return reinterpret_cast<Id>(&_);
}

} // namespace Karm::Meta
