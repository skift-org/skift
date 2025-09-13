#include <karm-logger/logger.h>
#include <karm-sys/endpoint.h>
#include <karm-sys/entry.h>

using namespace Karm;

namespace Strata::Dhcp {

Async::Task<> servAsync(Sys::Context& ctx) {
    auto endpoint = Sys::Endpoint::create(ctx);

    logInfo("service started");
    while (true) {
        co_trya$(endpoint.recvAsync());
        logDebug("received message from system");
    }
}

} // namespace Strata::Dhcp

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Strata::Dhcp::servAsync(ctx);
}
