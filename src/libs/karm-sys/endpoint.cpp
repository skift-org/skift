#include <karm-logger/logger.h>

#include "endpoint.h"

namespace Karm::Sys {

static Endpoint* _globalEndpoint = nullptr;

Endpoint::Endpoint(IpcConnection con)
    : _con(std::move(con)) {
    _globalEndpoint = this;
    // FIXME: Find a way to do proper cleanup
    Async::detach(_receiverTask(*this), [](Res<> res) {
        logError("receiver task exited: {}", res);
        panic("receiver task exited");
    });
}

Endpoint Endpoint::adopt(Context& ctx) {
    auto& channel = useChannel(ctx);
    return {std::move(channel.con)};
}

Endpoint& globalEndpoint() {
    if (not _globalEndpoint)
        panic("no active endpoint");
    return *_globalEndpoint;
}

} // namespace Karm::Sys
