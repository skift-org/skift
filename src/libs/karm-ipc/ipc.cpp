#include "ipc.h"

namespace Karm::Ipc {

// MARK: Server ----------------------------------------------------------------

Res<Server> Server::create(Sys::Context &ctx) {
    auto &channel = useChannel(ctx);
    return Ok(Server{std::move(channel.con)});
}

void Server::attach(_Object &obj) {
    _objects.put(obj._oid, &obj);
}

void Server::detach(_Object &obj) {
    _objects.del(obj._oid);
}

void Server::attach(_Pending &p) {
    _pending.put(p.seq, &p);
}

void Server::detach(_Pending &p) {
    _pending.del(p.seq);
}

Async::Task<> Server::runAsync() {
    Array<u8, 4096> reqBuf;
    Array<Sys::Handle, 16> hnds;

    while (true) {
        auto [bufLen, hndsLen] = co_trya$(_con.recvAsync(reqBuf, hnds));
        logInfo("received {} bytes and {} handles", bufLen, hndsLen);

        Io::PackScan req{
            sub(reqBuf, 0, bufLen),
            sub(hnds, 0, hndsLen),
        };

        Header header = co_try$(Io::unpack<Header>(req));
        auto maybeObject = _objects.tryGet(header.oid);

        if (maybeObject) {
            auto *object = *maybeObject;

            Io::BufferWriter respBuf;
            Io::PackEmit resp{respBuf};
            co_trya$(object->handleRequest(header, req, resp));
            co_trya$(_con.sendAsync(respBuf.bytes(), resp.handles()));
            continue;
        }

        auto maybePending = _pending.tryGet(header.seq);
        if (maybePending) {
            auto *pending = *maybePending;
            pending->complete(req);
            detach(*pending);
            continue;
        }

        logWarn("dropping message");
    }
}

// MARK: Object ----------------------------------------------------------------

static u64 _oidCounter = 0;

_Object::_Object(Server &server)
    : _oid(_oidCounter++),
      _server(server) {
    server.attach(*this);
}

_Object::~_Object() {
    _server.detach(*this);
}

// MARK: Client ----------------------------------------------------------------

usize Transport::_seq = 0;

} // namespace Karm::Ipc
