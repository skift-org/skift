#include <karm-sys/entry-async.h>
#include <karm-ui/app.h>

#include "../app.h"

Async::Task<> entryPointAsync(Sys::Ctx &ctx) {
    co_return Ui::runApp(
        ctx,
        Hideo::Colors::app() |
            Ui::pinSize({320, 200})
    );
}
