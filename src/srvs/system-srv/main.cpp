#include <karm-main/main.h>
#include <karm-sys/chan.h>

CliResult entryPoint(CliArgs) {
    Sys::println("Hello from userspace!");

    return Cli::SUCCESS;
}
