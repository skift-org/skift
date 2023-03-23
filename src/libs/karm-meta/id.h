#pragma once

#include <karm-base/panic.h>
#include <karm-base/std.h>

namespace Karm::Meta {

using Id = usize;

template <typename T>
static Id makeId() {
    return reinterpret_cast<Id>(__PRETTY_FUNCTION__ + 41);
}

template <typename T>
static char *makeName() {
    return __PRETTY_FUNCTION__ + 40;
}

template <typename T>
struct Type {
    using TYPE = T;

    static Id id() {
        return makeId<T>();
    }

    static char *name() {
        return makeName<T>();
    }
};

} // namespace Karm::Meta
