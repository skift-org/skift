#pragma once

#include <karm-main/base.h>
#include <karm-sys/chan.h>

int main(int argc, char const **argv) {
    CliArgs args{argc, argv};
    CliResult code = entryPoint(args);

    if (not code) {
        ::Karm::Error error = code.none();
        (void)::Karm::Fmt::format(::Karm::Sys::err(), "{}: {}\n", args.self(), error.msg());
        return 1;
    }

    return 0;
}
