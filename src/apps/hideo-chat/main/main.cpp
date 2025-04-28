#include <karm-sys/entry.h>

import Hideo.Chat;
import Karm.Ui;

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Ui::runAsync(
        ctx,
        Hideo::Chat::app()
    );
}
