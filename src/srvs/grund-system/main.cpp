#include <hjert-api/api.h>
#include <karm-logger/logger.h>
#include <karm-main/main.h>

#include "bus.h"

using namespace Grund::System;

Res<> entryPoint(Ctx &ctx) {
    try$(Hj::Task::self().label("grund-system"));

    auto bus = try$(Bus::create());

    auto deviceUnit = try$(Unit::load(ctx, "bundle://grund-device/_bin"_url));
    try$(bus.attach(deviceUnit));

    auto shellUnit = try$(Unit::load(ctx, "bundle://hideo-shell/_bin"_url));
    try$(bus.attach(shellUnit));

    return bus.run();
}
