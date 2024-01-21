#include <karm-sys/entry-async.h>
#include <web-http/fetch.h>

Async::Task<> entryPointAsync(Sys::Ctx &) {
    co_try_await$(Web::Client::fetch("http://www.google.com:80/"_url, Sys::out()));
    co_return Ok();
}
