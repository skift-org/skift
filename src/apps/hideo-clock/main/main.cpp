#include <karm-sys/entry-async.h>
#include <karm-ui/app.h>

#include "../app.h"

Async::Task<> entryPointAsync(Sys::Ctx &ctx) {
    auto app = Hideo::Clock::app();
    Async::detach(Hideo::Clock::timerTask(app));
    co_return Ui::runApp(ctx, app);
}
