#pragma once

#include <karm-base/std.h>

namespace Karm::Meta {

using Id = uintptr_t;

template <typename T>
struct _Id {
    static uint32_t _;
};

template <typename T>
static Id makeId() {
    return reinterpret_cast<Id>(&_Id<T>::_);
}

} // namespace Karm::Meta
