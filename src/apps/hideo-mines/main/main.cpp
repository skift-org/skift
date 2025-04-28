#include <karm-sys/entry.h>

import Karm.Ui;
import Hideo.Mines;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Ui::runAsync(ctx, Hideo::Mines::app());
}
