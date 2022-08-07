#include <karm-main/main.h>
#include <karm-test/driver.h>

CliResult entryPoint(CliArgs) {
    Test::driver().runAll();
    return Cli::SUCCESS;
}
