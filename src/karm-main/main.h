#ifdef __KARM_MAIN_INCLUDED
#    error "karm-main/main.h included twice"
#endif

#define __KARM_MAIN_INCLUDED

#include <karm-fmt/fmt.h>
#include <karm-sys/chan.h>

#include "base.h"

ExitCode entryPoint(CliArgs const &args);

int main(int argc, char const **argv) {
    CliArgs args{argc, argv};
    ExitCode code = entryPoint(args);

    if (!code) {
        Karm::Error error = code.none();
        (void)Karm::Fmt::format(Karm::Sys::err(), "{}: {}\n", args.self(), error.msg());
        return -1;
    }

    return code.take();
}
