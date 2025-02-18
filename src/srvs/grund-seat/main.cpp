#include <karm-rpc/base.h>
#include <karm-sys/entry.h>

namespace Grund::Seat {

Async::Task<> serv(Sys::Context& ctx) {
    auto endpoint = Rpc::Endpoint::create(ctx);

    logInfo("service started");
    while (true) {
        co_trya$(endpoint.recvAsync());
    }
}

} // namespace Grund::Seat

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    return Grund::Seat::serv(ctx);
}
