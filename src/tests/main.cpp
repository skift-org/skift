#include <karm-main/main.h>
#include <karm-test/driver.h>

ExitCode entryPoint(CliArgs const &) {
    Test::driver().runAll();
    return Cli::SUCCESS;
}
