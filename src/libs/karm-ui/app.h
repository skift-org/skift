#pragma once

#include <karm-cli/args.h>
#include <karm-sys/context.h>

#include "node.h"

namespace Karm::Ui {

Child inspector(Child child);

Res<> runApp(Sys::Context& ctx, Child root);

Async::Task<> runAsync(Sys::Context& ctx, Child root);

void mountApp(Cli::Command& cmd, Slot rootSlot);

} // namespace Karm::Ui
