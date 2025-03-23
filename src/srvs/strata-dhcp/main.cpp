#include <karm-rpc/base.h>
#include <karm-sys/entry.h>

namespace Strata::Dhcp {

Async::Task<> serv(Sys::Context& ctx) {
    auto endpoint = Rpc::Endpoint::create(ctx);

    logInfo("service started");
    while (true) {
        co_trya$(endpoint.recvAsync());
        logDebug("received message from system");
    }
}

} // namespace Strata::Dhcp

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    return Strata::Dhcp::serv(ctx);
}
