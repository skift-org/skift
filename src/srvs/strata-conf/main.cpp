#include <karm-rpc/base.h>
#include <karm-sys/entry.h>

namespace Strata::Conf {

Async::Task<> serv(Sys::Context& ctx) {
    auto endpoint = Rpc::Endpoint::create(ctx);

    logInfo("service started");
    while (true) {
        co_trya$(endpoint.recvAsync());
        logDebug("received message from system");
    }
}

} // namespace Strata::Conf

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    return Strata::Conf::serv(ctx);
}
