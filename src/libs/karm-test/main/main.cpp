#include <karm-sys/entry.h>
#include <karm-test/driver.h>

import Karm.Cli;

using namespace Karm;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto globArg = Cli::option<Str>(
        'g',
        "glob"s,
        "Run tests matching this glob pattern"s,
        "*"s
    );
    auto fastArg = Cli::flag(
        'f',
        "fast"s,
        "Stop on first failure"s
    );

    Cli::Command cmd{
        "karm-test"s,
        "Run the Karm test suite"s,
        {globArg, fastArg},
        [=](Sys::Context&) -> Async::Task<> {
            co_return co_await Test::driver().runAllAsync({
                .glob = globArg.unwrap(),
                .fast = fastArg.unwrap(),
            });
        },
    };

    co_return co_await cmd.execAsync(ctx);
}
