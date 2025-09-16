#include <karm-logger/logger.h>
#include <karm-sys/endpoint.h>
#include <karm-sys/entry.h>

using namespace Karm;

namespace Strata::Fs {

Async::Task<> servAsync(Sys::Context& ctx) {
    auto endpoint = Sys::Endpoint::adopt(ctx);

    logInfo("service started");
    while (true) {
        co_trya$(endpoint.recvAsync());
        logDebug("received message from system");
    }
}

} // namespace Strata::Fs

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Strata::Fs::servAsync(ctx);
}
