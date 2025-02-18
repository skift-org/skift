#include <karm-rpc/base.h>
#include <karm-sys/entry.h>

#include "../grund-bus/api.h"
#include "../grund-echo/api.h"

namespace Grund::Dns {

Async::Task<> serv(Sys::Context& ctx) {
    auto endpoint = Rpc::Endpoint::create(ctx);

    logDebug("sending nonsens to system");

    auto echoPort = co_trya$(endpoint.callAsync<Bus::Api::Locate>(Rpc::Port::BUS, "grund-echo"s));
    logDebug("located echo service at port: {}", echoPort);

    auto res = co_trya$(endpoint.callAsync<Echo::Api::Request>(echoPort, "nonsens"s));
    logDebug("received response from system: {}", res);

    logInfo("service started");
    while (true) {
        co_trya$(endpoint.recvAsync());
        logDebug("received message from system");
    }
}

} // namespace Grund::Dns

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    return Grund::Dns::serv(ctx);
}
