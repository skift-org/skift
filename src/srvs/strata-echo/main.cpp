#include <karm-sys/endpoint.h>
#include <karm-sys/entry.h>

#include "api.h"

namespace Strata::Echo {

Async::Task<> servAsync(Sys::Context& ctx) {
    auto endpoint = Sys::Endpoint::adopt(ctx);
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
    co_return co_await Strata::Echo::servAsync(ctx);
}
