#pragma once

#include <karm-sys/chan.h>
#include <karm-sys/context.h>

#include "abi.h"
#include "externs.h"

void __panicHandler(Karm::PanicKind kind, char const* msg);

extern "C" int wasm_export(wasm_main)(void) {
    Abi::Wasm::init();
    Karm::registerPanicHandler(__panicHandler);

    char const* self = "wasm-app";
    char const* argv[] = {self, nullptr};
    Sys::Context ctx;
    ctx.add<Sys::ArgsHook>(1, argv);
    Res<> code = Sys::run(entryPointAsync(ctx));

    if (not code) {
        Karm::Sys::errln("{}", code);
        return 1;
    }

    return 0;
}
