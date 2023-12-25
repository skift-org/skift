#pragma once

#include <karm-main/main.h>

#include "async.h"

Async::Prom<> entryPointAsync(Ctx &);

Res<> entryPoint(Ctx &ctx) {
    return try$(Async::runSync(entryPointAsync(ctx)));
}
