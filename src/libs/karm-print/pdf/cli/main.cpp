#include <karm-sys/entry.h>
#include <karm-sys/file.h>

Async::Task<> entryPointAsync(Sys::Context &) {
    co_return Ok();
}
