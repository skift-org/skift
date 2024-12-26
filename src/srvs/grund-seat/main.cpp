#include <karm-sys/entry.h>
#include <karm-sys/rpc.h>

namespace Grund::Seat {

Async::Task<> serv(Sys::Context &ctx) {
    auto rpc = Sys::Rpc::create(ctx);

    logInfo("service started");
    while (true) {
        co_trya$(rpc.recvAsync());
    }
}

} // namespace Grund::Seat

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    return Grund::Seat::serv(ctx);
}
