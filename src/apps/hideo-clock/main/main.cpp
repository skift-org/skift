#include <karm-sys/entry.h>
#include <karm-ui/app.h>

#include "../app.h"

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    Async::Cancelation c;
    auto app = Hideo::Clock::app();
    Async::detach(Hideo::Clock::timerTask(app, c.token()));
    co_return Ui::runApp(ctx, app);
}
