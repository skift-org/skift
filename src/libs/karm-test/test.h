#pragma once

#include <karm-base/loc.h>
#include <karm-base/res.h>
#include <karm-base/string.h>
#include <karm-meta/nocopy.h>

#include "_prelude.h"

#include "driver.h"

namespace Karm::Test {

struct Test : Meta::Static {
    using Func = Res<> (*)(Driver &);

    Str _name;
    Func _func;
    Loc _loc;

    Test(Str name, Func func, Loc loc = Loc::current())
        : _name(name), _func(func), _loc(loc) {
        driver().add(this);
    }

    Res<> run(Driver &driver) {
        return _func(driver);
    }
};

}; // namespace Karm::Test
