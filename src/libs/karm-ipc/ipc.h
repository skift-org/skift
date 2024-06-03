#pragma once

#include <karm-logger/logger.h>
#include <karm-sys/context.h>
#include <karm-sys/socket.h>

#include "hook.h"

namespace Karm::Ipc {

struct Server {
    Sys::IpcConnection _con;

    Server(Sys::IpcConnection con)
        : _con(std::move(con)) {}

    static Res<Server> create(Sys::Ctx &ctx) {
        auto &channel = useChannel(ctx);
        return Ok(Server{std::move(channel.con)});
    }

    Async::Task<> runAsync() {
        Array<u8, 4096> buf;
        Array<Sys::Handle, 16> hnds;
        while (true) {
            auto [bufLen, hndsLen] = co_trya$(_con.recvAsync(buf, hnds));
            logInfo("received {} bytes and {} handles", bufLen, hndsLen);
        }
    }
};

} // namespace Karm::Ipc
