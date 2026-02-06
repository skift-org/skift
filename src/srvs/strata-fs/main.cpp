#include <karm/entry>
#include <vaerk-handover/hook.h>

import Strata.Protos;
import Karm.Sys.Skift;
import Karm.Sys;
import Karm.Fs;
import Karm.Logger;
import Hjert.Api;

using namespace Karm;

namespace Strata::Fs {

struct FsSession : Sys::IpcSession {
    Rc<Karm::Fs::Node> _root;
    Map<IFs::Fid, Rc<Karm::Fs::Node>> _files = {};
    IFs::Fid _fids = 1;

    FsSession(Sys::IpcConnection conn, Rc<Karm::Fs::Node> root)
        : IpcSession(std::move(conn)),
          _root(root) {}

    Res<Rc<Karm::Fs::Node>> _resolveFid(IFs::Fid fid) {
        return _files.tryGet(fid).okOr(Error::invalidInput());
    }

    Async::Task<IFs::Open::Response> _handleOpenAsync(Sys::IpcMessage& message) {
        auto request = co_try$(message.unpack<IFs::Open>());
        auto file = co_trya$(_root->lookupAsync(Ref::Path{true, request.path}));
        auto fid = _fids++;
        _files.put(fid, file);
        co_return Ok(fid);
    }

    Async::Task<IFs::Close::Response> _handleCloseAsync(Sys::IpcMessage& message) {
        auto request = co_try$(message.unpack<IFs::Close>());
        if (not _files.has(request.fid))
            co_return Error::invalidInput();
        _files.del(request.fid);
        co_return Ok();
    }

    Async::Task<IFs::Read::Response> _handleReadAsync(Sys::IpcMessage& message) {
        auto request = co_try$(message.unpack<IFs::Read>());
        auto node = co_try$(_resolveFid(request.fid));
        Vec<u8> buf;
        buf.resize(request.len);
        co_trya$(node->readAsync(buf, request.off));
        co_return Ok(std::move(buf));
    }

    Async::Task<IFs::Write::Response> _handleWriteAsync(Sys::IpcMessage& message) {
        auto request = co_try$(message.unpack<IFs::Write>());
        auto node = co_try$(_resolveFid(request.fid));
        co_return co_await node->writeAsync(request.buf, request.off);
    }

    Async::Task<IFs::ReadDir::Response> _handleReadDirAsync(Sys::IpcMessage& message) {
        auto request = co_try$(message.unpack<IFs::ReadDir>());
        auto node = co_try$(_resolveFid(request.fid));
        co_return co_await node->listAsync();
    }

    Async::Task<IFs::Stat::Response> _handleStatAsync(Sys::IpcMessage& message) {
        auto request = co_try$(message.unpack<IFs::Write>());
        auto node = co_try$(_resolveFid(request.fid));
        co_return co_await node->statAsync();
    }

    Async::Task<> handleAsync(Sys::IpcMessage& msg, Async::CancellationToken) override {
        if (msg.is<IFs::Open>()) {
            co_try$(resp<IFs::Open>(msg, co_await _handleOpenAsync(msg)));
        } else if (msg.is<IFs::Close>()) {
            co_try$(resp<IFs::Close>(msg, co_await _handleCloseAsync(msg)));
        } else if (msg.is<IFs::Read>()) {
            co_try$(resp<IFs::Read>(msg, co_await _handleReadAsync(msg)));
        } else if (msg.is<IFs::Write>()) {
            co_try$(resp<IFs::Write>(msg, co_await _handleWriteAsync(msg)));
        } else if (msg.is<IFs::ReadDir>()) {
            co_try$(resp<IFs::ReadDir>(msg, co_await _handleReadDirAsync(msg)));
        } else if (msg.is<IFs::Stat>()) {
            co_try$(resp<IFs::Stat>(msg, co_await _handleStatAsync(msg)));
        }

        co_return Ok();
    }
};

struct FsHandler : Sys::IpcHandler {
    Rc<Karm::Fs::Node> _root;

    FsHandler(Rc<Karm::Fs::Node> root)
        : _root(root) {}

    Async::Task<Rc<Sys::IpcSession>> acceptSessionAsync(Sys::IpcConnection conn, Async::CancellationToken) override {
        co_return Ok(makeRc<FsSession>(std::move(conn), _root));
    }
};

} // namespace Strata::Fs

Async::Task<> entryPointAsync(Sys::Context& ctx, Async::CancellationToken ct) {
    auto& handover = useHandover(ctx);

    auto* bootfsRecord = handover.fileByName("file:/skift/init.bootfs");
    if (not bootfsRecord)
        co_return Error::notFound("no bootfs");

    auto vmo = co_try$(Hj::Vmo::create(Hj::ROOT, bootfsRecord->start, bootfsRecord->size, Hj::VmoFlags::DMA));
    co_try$(vmo.label("bootfs"));

    auto fd = makeRc<Sys::Skift::VmoFd>(std::move(vmo), 0);
    auto root = co_trya$(Karm::Fs::mountBootfsAsync(fd));

    auto handler = makeRc<Strata::Fs::FsHandler>(root);
    auto server = co_trya$(Sys::IpcServer::createAsync("ipc://strata-fs"_url, handler));
    co_return co_await server.servAsync(ct);
}
