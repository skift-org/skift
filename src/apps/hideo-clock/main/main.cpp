#include <karm-async/cancelation.h>
#include <karm-sys/entry.h>

import Hideo.Clock;
import Karm.Ui;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    Async::Cancelation c;
    auto app = Hideo::Clock::app();
    Async::detach(Hideo::Clock::timerTask(app, c.token()));
    co_return co_await Ui::runAsync(ctx, app);
}
