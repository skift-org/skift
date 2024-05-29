#include <karm-sys/entry.h>
#include <karm-test/driver.h>

Async::Task<> entryPointAsync(Sys::Ctx &) {
    return Test::driver().runAllAsync();
}
