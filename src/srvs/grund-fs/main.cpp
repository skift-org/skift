#include <karm-ipc/ipc.h>
#include <karm-sys/entry.h>

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto server = co_try$(Ipc::Server::create(ctx));

    logInfo("service started");
    co_return co_trya$(server.runAsync());
}
