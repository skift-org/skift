#include <karm-sys/entry.h>
#include <karm-ui/app.h>

import Hideo.About;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    return Ui::runAsync(ctx, Hideo::About::app());
}
