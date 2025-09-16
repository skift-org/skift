#include <karm-logger/logger.h>
#include <karm-sys/endpoint.h>
#include <karm-sys/entry.h>

#include "../strata-bus/api.h"
#include "../strata-echo/api.h"

namespace Strata::Dns {

Async::Task<> servAsync(Sys::Context& ctx) {
    auto endpoint = Sys::Endpoint::adopt(ctx);

    logDebug("sending nonsens to system");

    auto echoPort = co_trya$(endpoint.callAsync<Bus::Api::Locate>(Sys::Port::BUS, "strata-echo"s));
    logDebug("located echo service at port: {}", echoPort);

    auto res = co_trya$(endpoint.callAsync<Echo::Api::Request>(echoPort, "nonsens"s));
    logDebug("received response from system: {}", res);

    logInfo("service started");
    while (true) {
        co_trya$(endpoint.recvAsync());
        logDebug("received message from system");
    }
}

} // namespace Strata::Dns

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Strata::Dns::servAsync(ctx);
}
