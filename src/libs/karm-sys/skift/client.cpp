export module Karm.Sys.Skift:client;

import Karm.Core;
import Karm.Ref;
import Karm.Sys;
import Karm.Ipc;

using namespace Karm::Ref::Literals;

namespace Karm::Sys::Skift {

static Opt<Ipc::Client> _globalClient = NONE;

export void setupClient(IpcConnection conn) {
    _globalClient.emplace(std::move(conn));
}

export Ipc::Client& globalClient() {
    return _globalClient.unwrap();
}

} // namespace Karm::Sys::Skift
