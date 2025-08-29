#include <karm-sys/entry.h>
#include <karm-sys/time.h>

using namespace Karm;

Async::Task<> entryPointAsync(Sys::Context&) {
    Sys::println("{}", Sys::dateTime());
    co_return Ok();
}
