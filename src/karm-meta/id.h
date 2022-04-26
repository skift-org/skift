#pragma once

#include <karm-base/std.h>

namespace Karm::Meta {

using TypeId = uintptr_t;

template <typename T>
static TypeId type_id() {
    static uint32_t _;
    return reinterpret_cast<TypeId>(&_);
}

} // namespace Karm::Meta
