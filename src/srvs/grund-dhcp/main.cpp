#include <karm-sys/entry.h>
#include <karm-sys/rpc.h>

namespace Grund::Dhcp {

Async::Task<> serv(Sys::Context &ctx) {
    auto rpc = Sys::Rpc::create(ctx);

    logInfo("service started");
    while (true) {
        co_trya$(rpc.recvAsync());
        logDebug("received message from system");
    }
}

} // namespace Grund::Dhcp

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    return Grund::Dhcp::serv(ctx);
}
