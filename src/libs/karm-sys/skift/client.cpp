export module Karm.Sys.Skift:client;

import Karm.Core;
import Karm.Ref;
import Karm.Sys;

namespace Karm::Sys::Skift {

static Opt<IpcClient> _globalClient = NONE;
static Opt<IpcClient> _globalFsClient = NONE;

export void setupClient(IpcConnection conn) {
    _globalClient.emplace(std::move(conn));
}

export IpcClient& globalClient() {
    return _globalClient.unwrap();
}

export IpcClient& globalFsClient() {
    if (not _globalFsClient) {
        _globalFsClient.emplace(
            IpcConnection::connect("ipc://strata-fs"_url)
                .take("failed to connect to strata-fs")
        );
    }

    return _globalFsClient.unwrap();
}

} // namespace Karm::Sys::Skift
