#include <karm-sys/entry.h>

import Karm.Core;
import Karm.Ui;
import Hideo.Clock;

using namespace Karm;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto [cancelation, token] = Async::Cancellation::create();
    auto app = Hideo::Clock::app();
    Async::detach(Hideo::Clock::timerTask(app, token));
    co_return co_await Ui::runAsync(ctx, app);
}

// lilla
// kystrat
// spire
