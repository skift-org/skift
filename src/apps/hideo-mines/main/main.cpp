#include <karm-sys/entry.h>
#include <karm-ui/app.h>

import Hideo.Mines;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return Ui::runApp(
        ctx,
        Hideo::Mines::app()
    );
}
