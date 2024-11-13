#include <karm-sys/entry.h>
#include <karm-sys/rpc.h>

#include "api.h"

namespace Grund::Echo {

Async::Task<> serv(Sys::Context &ctx) {
    auto rpc = Sys::Rpc::create(ctx);
    while (true) {
        auto msg = co_trya$(rpc.recvAsync());
        if (msg.is<Echo::Request>()) {
            auto req = co_try$(msg.unpack<Echo::Request>());
            co_try$(rpc.resp<Echo::Request>(msg, Ok(req.msg)));
        }
    }

    co_return Ok();
}

} // namespace Grund::Echo

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    return Grund::Echo::serv(ctx);
}
