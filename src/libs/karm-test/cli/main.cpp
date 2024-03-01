#include <karm-sys/entry.h>
#include <karm-test/driver.h>

Res<> entryPoint(Sys::Ctx &) {
    return Test::driver().runAll();
}
