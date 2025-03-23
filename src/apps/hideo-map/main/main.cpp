#include <karm-sys/entry.h>
#include <karm-ui/app.h>

import Hideo.Map;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Ui::runAsync(ctx, Hideo::Map::app());
}
