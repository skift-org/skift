#include <karm-sys/entry.h>
#include <karm-ui/app.h>

import Hideo.Canvas;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return Ui::runApp(
        ctx,
        Hideo::Canvas::app()
    );
}
