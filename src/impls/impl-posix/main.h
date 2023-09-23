#pragma once

#include <karm-main/base.h>
#include <karm-sys/chan.h>

void __panicHandler(Karm::PanicKind kind, char const *msg);

int main(int argc, char const **argv) {
    Karm::registerPanicHandler(__panicHandler);

    Ctx ctx;
    ctx.add<ArgsHook>(argc, argv);
    Res<> code = entryPoint(ctx);

    if (not code) {
        ::Karm::Error error = code.none();
        (void)::Karm::Fmt::format(::Karm::Sys::err(), "{}: {}\n", argv[0], error.msg());
        return 1;
    }

    return 0;
}
