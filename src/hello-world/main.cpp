#include <karm-fmt/fmt.h>
#include <karm-main/main.h>
#include <karm-sys/chan.h>

ExitCode entryPoint(CliArgs const &) {
    try$(Fmt::format(Sys::out(), "Hello, world!\n"));
    return 0;
}
