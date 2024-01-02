#include <karm-async/main.h>
#include <web-client/fetch.h>

Async::Prom<> entryPointAsync(Ctx &) {
    co_try_await$(Web::Client::fetch("http://www.google.com:80/"_url, Sys::out()));
    co_return Ok();
}
