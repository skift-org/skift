#pragma once

#include "async.h"
#include "entry.h"

Sys::Task<> entryPointAsync(Sys::Ctx &);

[[gnu::weak]] Res<> entryPoint(Sys::Ctx &ctx) {
    return try$(Sys::runSync(entryPointAsync(ctx)));
}
