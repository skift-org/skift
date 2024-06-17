#include <karm-sys/entry.h>
#include <web-http/fetch.h>

Async::Task<> entryPointAsync(Sys::Context &) {
    co_trya$(Web::Client::fetch("http://www.google.com:80/"_url, Sys::out()));
    co_return Ok();
}
