#include <karm-rpc/base.h>
#include <karm-sys/entry.h>

namespace Grund::Fs {

Async::Task<> serv(Sys::Context &ctx) {
    auto endpoint = Rpc::Endpoint::create(ctx);

    logInfo("service started");
    while (true) {
        co_trya$(endpoint.recvAsync());
        logDebug("received message from system");
    }
}

} // namespace Grund::Fs

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    return Grund::Fs::serv(ctx);
}
