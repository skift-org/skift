#pragma once

#include <karm-meta/nocopy.h>

#include "base.h"

namespace Karm {

/// Defer a computation until the end of the current scope.
/// The computation is executed in the reverse order of the defer calls.
template <typename F>
struct Defer : Meta::Pinned {
    F _f;

    always_inline Defer(F f) : _f(f) {}

    always_inline ~Defer() {
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

    always_inline ArmedDefer(F f) : _f(f) {}

    always_inline ~ArmedDefer() {
        if (_armed)
            _f();
    }

    always_inline void disarm() {
        _armed = false;
    }

    always_inline void arm() {
        _armed = true;
    }
};

template <typename F>
ArmedDefer(F) -> ArmedDefer<F>;

} // namespace Karm
