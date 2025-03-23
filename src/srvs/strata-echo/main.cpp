#include <karm-rpc/base.h>
#include <karm-sys/entry.h>

#include "api.h"

namespace Strata::Echo {

Async::Task<> serv(Sys::Context& ctx) {
    auto endpoint = Rpc::Endpoint::create(ctx);
    while (true) {
        auto msg = co_trya$(endpoint.recvAsync());
        if (msg.is<Echo::Api::Request>()) {
            auto req = co_try$(msg.unpack<Echo::Api::Request>());
            co_try$(endpoint.resp<Echo::Api::Request>(msg, Ok(req.msg)));
        }
    }

    co_return Ok();
}

} // namespace Strata::Echo

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    return Strata::Echo::serv(ctx);
}
