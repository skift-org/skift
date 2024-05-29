#include <hjert-api/api.h>
#include <karm-logger/logger.h>
#include <karm-sys/entry-async.h>

#include "bus.h"

using namespace Grund::System;

Async::Task<> entryPointAsync(Sys::Ctx &ctx) {
    co_try$(Hj::Task::self().label("grund-system"));

    auto bus = co_try$(Bus::create());

    auto deviceUnit = co_try$(Unit::load(ctx, "bundle://grund-device/_bin"_url));
    co_try$(bus.attach(deviceUnit));

    auto shellUnit = co_try$(Unit::load(ctx, "bundle://hideo-shell.main/_bin"_url));
    co_try$(bus.attach(shellUnit));

    co_return bus.run();
}
