#pragma once

#include "async.h"
#include "entry.h"

Async::Task<> entryPointAsync(Sys::Ctx &);

[[gnu::weak]] Res<> entryPoint(Sys::Ctx &ctx) {
    return try$(Sys::run(entryPointAsync(ctx)));
}
