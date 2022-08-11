#include <karm-main/main.h>

CliResult entryPoint(CliArgs) {
    Sys::println("Hello, world!");
    return Cli::SUCCESS;
}
