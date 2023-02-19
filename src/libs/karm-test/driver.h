#pragma once

#include <karm-base/loc.h>
#include <karm-base/vec.h>
#include <karm-sys/chan.h>

namespace Karm::Test {

struct Test;

struct Driver {
    Vec<Test *> _tests;

    bool beginDescribe(Str name, Loc = Loc::current());

    void endDescribe();

    bool beginIt(Str name, Loc = Loc::current());

    void endIt();

    void add(Test *test);

    void runAll();

    Res<> unexpect(auto const &__lhs, auto const &__rhs, Str op, Loc = Loc::current()) {
        Sys::errln("Test Failled: {} {} {}", __lhs, op, __rhs);
        return Error::other("test Failled");
    }
};

Driver &driver();

} // namespace Karm::Test
