#include <karm-sys/entry.h>
#include <karm-ui/app.h>

#include "../app.h"

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    co_return Ui::runApp(
        ctx,
        Hideo::Sysmon::app()
    );
}
