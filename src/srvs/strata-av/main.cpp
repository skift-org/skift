#include <karm-rpc/base.h>
#include <karm-sys/entry.h>

namespace Strata::Av {

Async::Task<> servAsync(Sys::Context& ctx) {
    auto endpoint = Rpc::Endpoint::create(ctx);

    logInfo("service started");
    while (true) {
        co_trya$(endpoint.recvAsync());
        logDebug("received message from system");
    }
}

} // namespace Strata::Av

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Strata::Av::servAsync(ctx);
}
