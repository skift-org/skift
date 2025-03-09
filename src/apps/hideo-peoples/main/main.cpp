#include <karm-sys/entry.h>
#include <karm-ui/app.h>

import Hideo.Peoples;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return Ui::runApp(
        ctx,
        Hideo::Peoples::app()
    );
}
