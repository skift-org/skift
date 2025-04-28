#include <karm-sys/entry.h>

import Hideo.Counter;
import Karm.Ui;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Ui::runAsync(ctx, Hideo::Counter::app());
}
