#include <karm-sys/entry.h>
#include <karm-sys/rpc.h>

namespace Grund::Conf {

Async::Task<> serv(Sys::Context &ctx) {
    auto rpc = Sys::Rpc::create(ctx);

    logInfo("service started");
    while (true) {
        co_trya$(rpc.recvAsync());
        logDebug("received message from system");
    }
}

} // namespace Grund::Conf

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    return Grund::Conf::serv(ctx);
}
