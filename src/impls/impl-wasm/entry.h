#pragma once

#include <karm-sys/chan.h>
#include <karm-sys/context.h>

#include "externs.h"
#include "abi.h"


void __panicHandler(Karm::PanicKind kind, char const *msg);

extern "C" int wasm_export(wasm_main)(void) {
    Abi::Wasm::init();
    Karm::registerPanicHandler(__panicHandler);
    auto &ctx = Sys::globalCtx();
    Res<> code = entryPoint(ctx);


    if (not code) {
        Karm::Sys::errln("{}", code);
        return 1;
    }

    return 0;
}