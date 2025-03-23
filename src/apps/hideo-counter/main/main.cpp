#include <karm-sys/entry.h>
#include <karm-ui/app.h>

#include "../app.h"

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Ui::runAsync(ctx, Hideo::Counter::app());
}
