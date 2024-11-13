#include <karm-sys/entry.h>
#include <karm-sys/rpc.h>

#include "../grund-bus/api.h"
#include "../grund-echo/api.h"

namespace Grund::Dns {

Async::Task<> serv(Sys::Context &ctx) {
    auto rpc = Sys::Rpc::create(ctx);

    logDebug("sending nonsens to system");

    auto echoPort = co_trya$(rpc.callAsync<Bus::Locate>(Sys::Port::BUS, "grund-echo"s));
    logDebug("located echo service at port: {}", echoPort);

    auto res = co_trya$(rpc.callAsync<Echo::Request>(echoPort, "nonsens"s));
    logDebug("received response from system: {}", res);

    logInfo("service started");
    while (true) {
        co_trya$(rpc.recvAsync());
        logDebug("received message from system");
    }
}

} // namespace Grund::Dns

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    return Grund::Dns::serv(ctx);
}
