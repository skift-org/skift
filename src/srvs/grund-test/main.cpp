#include <grund-echo/api.h>
#include <karm-ipc/client.h>
#include <karm-logger/logger.h>
#include <karm-sys/entry-async.h>

Async::Task<> entryPointAsync(Sys::Ctx &) {
    auto client = co_trya$(Ipc::connectAsync<Grund::Echo::IEcho>("ipc:/echo"_url));
    auto message = co_trya$(client->pingAsync("Hello, world!"));
    logInfo("Got message: {}", message);
    co_return Ok();
}
