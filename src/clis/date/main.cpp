#include <karm-sys/entry-async.h>
#include <karm-sys/time.h>

Async::Task<> entryPointAsync(Sys::Ctx &) {
    Sys::println("{}", Sys::dateTime());
    co_return Ok();
}
