#pragma once

#include <karm-meta/nocopy.h>

#include "macros.h"

namespace Karm {

#define defer$(expr) \
    auto _ = Defer { \
        [&] {        \
            expr;    \
        }            \
    }

template <typename F>
struct Defer : public Meta::Static {
    F _f;

    Defer(F f) : _f(f) {
    }

    ~Defer() {
        _f();
    }
};

template <typename F>
Defer(F) -> Defer<F>;

} // namespace Karm
