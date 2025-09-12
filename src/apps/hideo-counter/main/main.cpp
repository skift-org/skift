#include <karm-sys/entry.h>

import Hideo.Counter;
import Karm.Ui;

using namespace Karm;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Ui::runAsync(ctx, Hideo::Counter::app());
}
