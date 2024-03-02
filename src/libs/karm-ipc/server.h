#pragma once

#include "base.h"

namespace Karm::Ipc {

Async::Task<> serveLoop(Sys::IpcConnection &con, Func<Async::Task<>(Io::BScan &req, Io::BEmit &resp)> handle) {
    Buf<Byte> reqBuf;
    Buf<Sys::Handle> reqHnds;
    reqBuf.resize(Sys::IpcConnection::MAX_BUF_SIZE);
    reqHnds.resize(Sys::IpcConnection::MAX_HND_SIZE);

    while (true) {
        [[maybe_unused]] auto [nbytes, _] = co_trya$(con.recvAsync(reqBuf, reqHnds));
        logDebug("Got {} bytes from the client", nbytes);

        Io::BScan reqScan{sub(reqBuf, 0, nbytes)};

        Io::BufferWriter respData;
        Io::BEmit respEmit{respData};

        co_trya$(con.sendAsync(respData.bytes(), {}));
    }
}

struct Handler {
    u64 mid;
    Io::BScan &req;
    Io::BEmit &resp;

    template <typename... Args>
    Async::Task<> call(auto invoke) {
        logDebug("Handling message id: {016x}", mid);
        auto args = Io::unpack<Tuple<Args...>>(req);
        auto ret = co_await args.apply(invoke);
        Io::pack(resp, ret);
        co_return Ok();
    }

    Async::Task<> error() {
        logWarn("Invalid message id: {016x}", mid);
        resp.writeU64be(Message::ERROR);
        resp.writeU64be(Message::END);
        co_return Ok();
    }
};

template <Interface I>
Async::Task<> handleConnection(Sys::IpcConnection con) {
    I i;
    co_return co_trya$(serveLoop(con, [&](Io::BScan &req, Io::BEmit &resp) -> Async::Task<> {
        u64 iId = req.nextU64le();
        u64 mId = req.nextU64le();

        if (iId != I::_UID) {
            logWarn("Invalid interface id: {016x}", iId);
            resp.writeU64be(Message::ERROR);
            resp.writeU64be(Message::END);
        }

        logDebug("Handling interface id: {016x}", iId);

        Handler handler{mId, req, resp};
        co_trya$(i._dispatch(handler));
        resp.writeU64le(Message::END);

        co_return Ok();
    }));
}

template <Interface I>
Async::Task<> serveAsync(Sys::Ctx &ctx, Url::Url url) {
    auto listener = co_try$(Sys::IpcListener::listen(url));
    logInfo("Serving {} on {}...", I::_NAME, url);
    while (true)
        Async::detach(handleConnection<I>(co_trya$(listener.acceptAsync())));
}

} // namespace Karm::Ipc
