#include <karm-sys/entry.h>

import Hideo.Calculator;
import Karm.Ui;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Ui::runAsync(ctx, Hideo::Calculator::app());
}
