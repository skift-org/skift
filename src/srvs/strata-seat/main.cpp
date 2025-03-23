#include <karm-rpc/base.h>
#include <karm-sys/entry.h>

namespace Strata::Seat {

Async::Task<> serv(Sys::Context& ctx) {
    auto endpoint = Rpc::Endpoint::create(ctx);

    logInfo("service started");
    while (true) {
        co_trya$(endpoint.recvAsync());
    }
}

} // namespace Strata::Seat

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    return Strata::Seat::serv(ctx);
}
