#include <karm-sys/entry.h>
#include <karm-sys/ipc.h>

#include "../grund-echo/api.h"

namespace Grund::Dns {

Async::Task<> serv(Sys::Context &ctx) {
    Sys::Ipc ipc = Sys::Ipc::create(ctx);

    logDebug("sending nonsens to system");
    auto res = co_trya$(ipc.callAsync<Echo::Request>({}, "nonsens"s));
    logDebug("received response from system: ", res);

    logInfo("service started");
    while (true) {
        co_trya$(ipc.recvAsync());
        logDebug("received message from system");
    }
}

} // namespace Grund::Dns

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    return Grund::Dns::serv(ctx);
}
