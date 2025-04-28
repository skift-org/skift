#pragma once

#include <karm-sys/chan.h>
#include <karm-sys/context.h>

#define EXIT_FAILURE 1 /* Failing exit status.  */
#define EXIT_SUCCESS 0 /* Successful exit status.  */

void __panicHandler(Karm::PanicKind kind, char const* msg);

int main(int argc, char const** argv) {
    Karm::registerPanicHandler(__panicHandler);

    Sys::Context ctx;
    ctx.add<Sys::ArgsHook>(argc, argv);
    Res<> code = Sys::run(entryPointAsync(ctx));
    if (not code) {
        Karm::Sys::errln("{}: {}", argv[0], code);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
