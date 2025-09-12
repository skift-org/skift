module;

#include <karm-core/macros.h>

export module Karm.Core:base.manual;

import :meta.traits;
import :base.base;

namespace Karm {

export template <typename T>
struct Manual {
    alignas(alignof(T)) char _inner[sizeof(T)];

    template <typename... Args>
    always_inline void ctor(Args&&... args) {
        new (&unwrap()) T(std::forward<Args>(args)...);
    }

    always_inline void dtor() {
        unwrap().~T();
    }

    always_inline T& unwrap() {
        return *(T*)_inner;
    }

    always_inline T const& unwrap() const {
        return *(T const*)_inner;
    }

    always_inline T take() {
        T value = std::move(unwrap());
        dtor();
        return value;
    }
};

static_assert(Meta::Trivial<Manual<isize>>);
static_assert(sizeof(Manual<isize>) == sizeof(isize));
static_assert(alignof(Manual<isize>) == alignof(isize));

} // namespace Karm
