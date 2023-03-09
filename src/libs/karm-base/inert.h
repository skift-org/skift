#pragma once

#include <karm-meta/traits.h>

#include "std.h"

namespace Karm {

template <typename T>
union Inert {
    alignas(alignof(T)) char _inner[sizeof(T)];

    template <typename... Args>
    void ctor(Args &&...args) {
        new (&unwrap()) T(std::forward<Args>(args)...);
    }

    void dtor() {
        unwrap().~T();
    }

    T &unwrap() {
        return *(T *)_inner;
    }

    T const &unwrap() const {
        return *(T const *)_inner;
    }

    T take() {
        T value = std::move(unwrap());
        dtor();
        return value;
    }
};

static_assert(Meta::Trivial<Inert<isize>>);

}; // namespace Karm
