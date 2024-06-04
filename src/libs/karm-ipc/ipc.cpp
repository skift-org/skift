#include "ipc.h"

namespace Karm::Ipc {

// MARK: Server ----------------------------------------------------------------

Res<Server> Server::create(Sys::Ctx &ctx) {
    auto &channel = useChannel(ctx);
    return Ok(Server{std::move(channel.con)});
}

void Server::attach(_Object &obj) {
    _objects.put(obj._oid, &obj);
}

void Server::detach(_Object &obj) {
    _objects.del(obj._oid);
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

        Io::BufferWriter respBuf;
        Io::PackEmit resp{respBuf};

        Header header = co_try$(Io::unpack<Header>(req));
        auto object = co_try$(_objects.get(header.oid));
        co_trya$(object->handleRequest(header, req, resp));
        co_trya$(_con.sendAsync(respBuf.bytes(), resp.handles()));
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

} // namespace Karm::Ipc
