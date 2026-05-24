export module Karm.Sys.Skift:client;

import Karm.Core;
import Karm.Ref;
import Karm.Sys;
import Karm.Ipc;

using namespace Karm::Ref::Literals;

namespace Karm::Sys::Skift {

static Opt<Ipc::Client> _globalClient = NONE;
static Opt<Ipc::Client> _globalFsClient = NONE;

export void setupClient(IpcConnection conn) {
    _globalClient.emplace(std::move(conn));
}

export Ipc::Client& globalClient() {
    return _globalClient.unwrap();
}

export Ipc::Client& globalFsClient() {
    if (not _globalFsClient) {
        _globalFsClient.emplace(
            IpcConnection::connect("ipc://strata-fs"_url)
                .take("failed to connect to strata-fs")
        );
    }

    return _globalFsClient.unwrap();
}

} // namespace Karm::Sys::Skift
