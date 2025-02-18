#include <karm-sys/entry.h>
#include <karm-ui/app.h>

#include "../app.h"

Async::Task<> entryPointAsync(Sys::Context& ctx) {
#ifdef __ck_sys_skift__
    co_return co_await Ui::runAsync(ctx, Hideo::Calculator::app());
#else
    co_return Ui::runApp(ctx, Hideo::Calculator::app());
#endif
}
