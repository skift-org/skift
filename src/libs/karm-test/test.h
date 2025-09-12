#pragma once

import Karm.Core;

#include "_prelude.h"
#include "driver.h"

namespace Karm::Test {

struct Test : Meta::Pinned {
    enum struct Kind {
        SYNC,
        ASYNC,
    };

    using enum Kind;
    using Func = Res<> (*)(Driver&);
    using FuncAsync = Async::Task<> (*)(Driver&);

    Str _name;
    Kind _kind;

    union {
        Func _func;
        FuncAsync _funcAsync;
    };

    Loc _loc;

    Test(Str name, Func func, Loc loc = Loc::current())
        : _name(name), _kind(SYNC), _func(func), _loc(loc) {
        driver().add(this);
    }

    Test(Str name, FuncAsync func, Loc loc = Loc::current())
        : _name(name), _kind(ASYNC), _funcAsync(func), _loc(loc) {
        driver().add(this);
    }

    Async::Task<> runAsync(Driver& driver) {
        if (_kind == ASYNC) {
            co_return co_await _funcAsync(driver);
        } else {
            co_return _func(driver);
        }
    }
};

} // namespace Karm::Test
