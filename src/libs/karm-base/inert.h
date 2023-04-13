#pragma once

#include <karm-meta/traits.h>

#include "macros.h"
#include "std.h"

namespace Karm {

template <typename T>
union Inert {
    alignas(alignof(T)) char _inner[sizeof(T)];

    template <typename... Args>
    ALWAYS_INLINE void ctor(Args &&...args) {
        new (&unwrap()) T(std::forward<Args>(args)...);
    }

    ALWAYS_INLINE void dtor() {
        unwrap().~T();
    }

    ALWAYS_INLINE T &unwrap() {
        return *(T *)_inner;
    }

    ALWAYS_INLINE T const &unwrap() const {
        return *(T const *)_inner;
    }

    ALWAYS_INLINE T take() {
        T value = std::move(unwrap());
        dtor();
        return value;
    }
};

static_assert(Meta::Trivial<Inert<isize>>);

}; // namespace Karm
