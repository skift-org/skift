#include <karm-main/main.h>
#include <karm-test/driver.h>

Res<> entryPoint(Ctx &ctx) {
    Test::driver().runAll();
    return Ok();
}
