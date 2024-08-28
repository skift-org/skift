#include <karm-sys/entry.h>

Async::Task<> entryPointAsync(Sys::Context &) {
    Sys::println("Hello, World!");
    co_return Ok();
}
