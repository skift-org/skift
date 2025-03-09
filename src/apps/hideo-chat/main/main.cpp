#include <karm-sys/entry.h>
#include <karm-ui/app.h>

import Hideo.Chat;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return Ui::runApp(
        ctx,
        Hideo::Chat::app()
    );
}
