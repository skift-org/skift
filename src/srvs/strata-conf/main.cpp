#include <karm-sys/endpoint.h>
#include <karm-sys/entry.h>
#include <karm-logger/logger.h>

using namespace Karm;

namespace Strata::Conf {

Async::Task<> servAsync(Sys::Context& ctx) {
    auto endpoint = Sys::Endpoint::create(ctx);

    logInfo("service started");
    while (true) {
        co_trya$(endpoint.recvAsync());
        logDebug("received message from system");
    }

    co_return Ok();
}

} // namespace Strata::Conf

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Strata::Conf::servAsync(ctx);
}
