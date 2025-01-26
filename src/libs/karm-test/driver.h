#pragma once

#include <karm-base/loc.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>
#include <karm-sys/chan.h>

namespace Karm::Test {

struct Test;

struct Driver {
    Vec<Test*> _tests;

    void add(Test* test);

    Async::Task<> runAllAsync();

    Res<> unexpect(auto const& lhs, auto const& rhs, Str op, Loc loc = Loc::current()) {
        logError({"unexpected: {#} {} {#}", loc}, lhs, op, rhs);
        return Error::other("unexpected");
    }
};

Driver& driver();

} // namespace Karm::Test
