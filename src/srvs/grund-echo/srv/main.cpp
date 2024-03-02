#include <karm-ipc/server.h>
#include <karm-logger/logger.h>
#include <karm-sys/entry-async.h>

#include "../api.h"

namespace Grund::Echo {

struct EchoServer : public IEcho {
    Async::Task<String> pingAsync(String val) override {
        logInfo("Got message: {}", val);
        co_return Ok(Io::toUpperCase(val));
    }
};

} // namespace Grund::Echo

Async::Task<> entryPointAsync(Sys::Ctx &ctx) {
    return Ipc::serveAsync<Grund::Echo::EchoServer>(ctx, "ipc:/echo"_url);
}
