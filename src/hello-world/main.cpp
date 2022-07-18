#include <karm-debug/logger.h>
#include <karm-main/main.h>

ExitCode entryPoint(CliArgs const &) {
    Str name = "world";

    Sys::println("Hello, {}!", name);
    Debug::ldebug("Hello, {}!", name);
    Debug::linfo("Hello, {}!", name);
    Debug::lwarn("Hello, {}!", name);
    Debug::lerror("Hello, {}!", name);
    Debug::lpanic("Hello, {}!", name);

    return Cli::SUCCESS;
}
