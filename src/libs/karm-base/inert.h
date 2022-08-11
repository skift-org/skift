#pragma once

#include <karm-meta/traits.h>

#include "_prelude.h"

#include "std.h"

namespace Karm {

template <typename T>
union Inert {
    alignas(alignof(T)) char _inner[sizeof(T)];

    void ctor(T &&value) {
        new (&unwrap()) T(std::move(value));
    }

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

static_assert(Meta::Trivial<Inert<int>>, "Inert is not trivial");

}; // namespace Karm
