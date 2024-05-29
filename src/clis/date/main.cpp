#include <karm-sys/entry.h>
#include <karm-sys/time.h>

Async::Task<> entryPointAsync(Sys::Ctx &) {
    Sys::println("{}", Sys::dateTime());
    co_return Ok();
}
