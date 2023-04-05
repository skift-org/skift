#pragma once

#include <karm-base/loc.h>
#include <karm-base/vec.h>
#include <karm-sys/chan.h>

namespace Karm::Test {

struct Test;

struct Driver {
    Vec<Test *> _tests;

    void add(Test *test);

    void runAll();

    Res<> unexpect(auto const &__lhs, auto const &__rhs, Str op, Loc = Loc::current()) {
        Sys::errln("unexpected: '{}' {} '{}'", __lhs, op, __rhs);
        return Error::other("unexpected");
    }
};

Driver &driver();

} // namespace Karm::Test
