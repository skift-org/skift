#include <karm-net/http/fetch.h>
#include <karm-sys/entry.h>

Async::Task<> entryPointAsync(Sys::Context&) {
    co_trya$(Net::Http::fetch("http://www.google.com:80/"_url, Sys::out()));
    co_return Ok();
}
