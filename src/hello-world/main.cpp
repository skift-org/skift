#include <karm-main/main.h>

ExitCode entryPoint(CliArgs const &) {
    Sys::println("Hello, world!");
    return Cli::SUCCESS;
}
