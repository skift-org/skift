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
        ::Karm::Error error = code.none();
        (void)::Karm::Fmt::format(::Karm::Sys::err(), "{}: {}\n", argv[0], error.msg());
        return 1;
    }

    return 0;
}
