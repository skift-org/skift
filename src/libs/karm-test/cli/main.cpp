#include <karm-sys/entry.h>
#include <karm-test/driver.h>

Async::Task<> entryPointAsync(Sys::Context&) {
    co_return co_await Test::driver().runAllAsync();
}
