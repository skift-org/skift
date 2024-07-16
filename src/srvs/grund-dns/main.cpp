#include <grund-echo/api.h>
#include <karm-ipc/ipc.h>
#include <karm-sys/entry.h>

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto server = co_try$(Ipc::Server::create(ctx));

    logInfo("service started");

    logDebug("sending nonsense to system ---------------------------------------");

    auto object = Ipc::open<Grund::IEcho>(server, 0);
    auto res = co_trya$(object->echoAsync("hello"s));

    logDebug("received response from system -----------------------------------");

    co_return co_trya$(server.runAsync());
}
