#include <karm-main/main.h>
#include <karm-test/driver.h>

Res<> entryPoint(CliArgs) {
    Test::driver().runAll();
    return Ok();
}
