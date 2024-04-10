#pragma once

#include <karm-meta/nocopy.h>

#include "macros.h"
#include "std.h"

namespace Karm {

#define defer$(expr) \
    auto _ = Defer { \
        [&] {        \
            expr;    \
        }            \
    }

/// Defer a computation until the end of the current scope.
/// The computation is executed in the reverse order of the defer calls.
template <typename F>
struct Defer : Meta::Static {
    F _f;

    Defer(F f) : _f(f) {}
    ~Defer() {
        _f();
    }
};

template <typename F>
Defer(F) -> Defer<F>;

/// Defer a computation until the end of the current scope.
/// The computation is executed in the reverse order of the defer calls.
/// Can be disarm which will prevent the computation from being executed.
template <typename F>
struct ArmedDefer : Meta::NoCopy {
    F _f;
    bool _armed = true;

    ArmedDefer(F f) : _f(f) {}

    ~ArmedDefer() {
        if (_armed)
            _f();
    }

    void disarm() {
        _armed = false;
    }
};

template <typename F>
ArmedDefer(F) -> ArmedDefer<F>;

} // namespace Karm
