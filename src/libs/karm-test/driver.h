#pragma once

import Karm.Core;

#include <karm-logger/logger.h>
#include <karm-sys/chan.h>

namespace Karm::Test {

struct Test;

struct RunOptions {
    String glob = "*"s;
    bool fast = false;
};

struct Driver {
    Vec<Test*> _tests;

    void add(Test* test);

    Async::Task<> runAllAsync(RunOptions options);

    Res<> unexpect(auto const& lhs, auto const& rhs, Str op, Loc loc = Loc::current()) {
        logError({"unexpected: {#} {} {#}", loc}, lhs, op, rhs);
        return Error::other("unexpected");
    }
};

Driver& driver();

} // namespace Karm::Test
