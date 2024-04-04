#pragma once

#include <karm-sys/chan.h>
#include <karm-sys/context.h>

void __panicHandler(Karm::PanicKind kind, char const *msg);

int main(int argc, char const **argv) {
    Karm::registerPanicHandler(__panicHandler);

    auto &ctx = Sys::globalCtx();
    ctx.add<Sys::ArgsHook>(argc, argv);
    Res<> code = entryPoint(ctx);
    if (not code) {
        Karm::Sys::errln("{}: {}", argv[0], code);
        return 1;
    }
    return 0;
}
