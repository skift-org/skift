#include <karm-sys/entry.h>

import Karm.Sys;
import Karm.Core;
import Karm.Cli;
import Strata.Init;

using namespace Karm;

Async::Task<> entryPointAsync(Sys::Context&) {
    auto um = Strata::Init::UnitManager{};
    auto units = co_try$(um.listUnits());
    Sys::println("--- Units ---\n{}", units);
    co_return Ok();
}
