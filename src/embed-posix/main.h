#pragma once

#include <karm-fmt/fmt.h>
#include <karm-main/base.h>
#include <karm-sys/chan.h>

int main(int argc, char const **argv) {
    CliArgs args{argc, argv};
    ExitCode code = entryPoint(args);

    if (!code) {
        ::Karm::Error error = code.none();
        (void)::Karm::Fmt::format(::Karm::Sys::err(), "{}: {}\n", args.self(), error.msg());
        return 1;
    }

    return 0;
}
