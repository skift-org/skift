#include <karm-sys/entry.h>
#include <karm-ui/app.h>

import Hideo.Code;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return Ui::runApp(
        ctx,
        Hideo::Code::app()
    );
}
