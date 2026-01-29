export module Karm.Sys.Skift:client;

import Karm.Core;
import Karm.Sys;

namespace Karm::Sys::Skift {

static Opt<IpcClient> _globalClient = NONE;

export void setupClient(IpcConnection conn) {
    _globalClient.emplace(std::move(conn));
}

export IpcClient& globalClient() {
    return _globalClient.unwrap();
}

} // namespace Karm::Sys::Skift