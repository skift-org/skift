#include <karm-sys/entry.h>
#include <karm-test/driver.h>

Res<> entryPoint(Sys::Ctx &) {
    Test::driver().runAll();
    return Ok();
}
