#include <karm-sys/entry.h>
#include <karm-test/driver.h>

Async::Task<> entryPointAsync(Sys::Context &) {
    return Test::driver().runAllAsync();
}
