#include <karm-sys/entry.h>
#include <karm-sys/ipc.h>

#include "api.h"

namespace Grund::Echo {

Async::Task<> serv(Sys::Context &ctx) {
    Sys::Ipc ipc = Sys::Ipc::create(ctx);
    while (true) {
        auto msg = co_trya$(ipc.recvAsync());
        if (msg.is<Echo::Request>()) {
            auto req = co_try$(msg.unpack<Echo::Request>());
            co_try$(ipc.resp<Echo::Request>(msg, Ok(req.msg)));
        }
    }

    co_return Ok();
}

} // namespace Grund::Echo

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    return Grund::Echo::serv(ctx);
}
