#include <karm/entry>

import Karm.Fs;
import Karm.Ipc;
import Karm.Logger;
import Karm.Sys.Skift;
import Karm.Sys;

import Hjert.Api;
import Strata.Protos;

using namespace Karm;
using namespace Karm::Ref::Literals;

namespace Strata::Fs {

struct FsNodeHandler : Ipc::Session {
    Rc<Karm::Fs::Node> _node;

    FsNodeHandler(Sys::IpcConnection conn, Rc<Karm::Fs::Node> node)
        : Session(std::move(conn)),
          _node(node) {}

    Async::Task<IFs::Read::Response> _handleReadAsync(Ipc::Message& message) {
        auto request = co_try$(message.unpack<IFs::Read>());
        Vec<u8> buf;
        buf.resize(min(request.len, Ipc::MAX_TRANSFER_SIZE));
        auto read = co_trya$(_node->readAsync(buf, request.off));
        buf.trunc(read);
        co_return Ok<IFs::Read::Response>(std::move(buf));
    }

    Async::Task<IFs::Write::Response> _handleWriteAsync(Ipc::Message& message) {
        auto request = co_try$(message.unpack<IFs::Write>());
        co_return co_await _node->writeAsync(request.buf, request.off);
    }

    Async::Task<IFs::ReadDir::Response> _handleReadDirAsync(Ipc::Message&) {
        co_return co_await _node->listAsync();
    }

    Async::Task<IFs::Stat::Response> _handleStatAsync(Ipc::Message&) {
        co_return co_await _node->statAsync();
    }

    Async::Task<IFs::Mmap::Response> _handleMmapAsync(Ipc::Message&) {
        co_return _node->underlying();
    }

    Async::Task<> handleAsync(Ipc::Message& msg, Async::CancellationToken) override {
        if (msg.is<IFs::Read>()) {
            co_try$(resp<IFs::Read>(msg, co_await _handleReadAsync(msg)));
        } else if (msg.is<IFs::Write>()) {
            co_try$(resp<IFs::Write>(msg, co_await _handleWriteAsync(msg)));
        } else if (msg.is<IFs::ReadDir>()) {
            co_try$(resp<IFs::ReadDir>(msg, co_await _handleReadDirAsync(msg)));
        } else if (msg.is<IFs::Stat>()) {
            co_try$(resp<IFs::Stat>(msg, co_await _handleStatAsync(msg)));
        } else if (msg.is<IFs::Mmap>()) {
            co_try$(resp<IFs::Mmap>(msg, co_await _handleMmapAsync(msg)));
        }

        co_return Ok();
    }
};

struct FsHandler : Ipc::Handler {
    Rc<Karm::Fs::Node> _root;

    FsHandler(Rc<Karm::Fs::Node> root)
        : _root(root) {}

    Async::Task<Rc<Ipc::Session>> acceptSessionAsync(Sys::IpcConnection conn, Async::CancellationToken) override {
        logDebug("open: {}", conn._url);
        auto node = co_trya$(_root->lookupAsync(conn.url().path));
        debug("ok");
        co_return Ok(makeRc<FsNodeHandler>(std::move(conn), node));
    }
};

} // namespace Strata::Fs

Async::Task<> entryPointAsync(Sys::Env&, Async::CancellationToken ct) {
    auto& handover = Sys::Skift::useHandover();

    auto* bootfsRecord = handover.blobByName("file:/skift/init.bootfs");
    if (not bootfsRecord)
        co_return Error::notFound("no bootfs");

    auto vmo = co_try$(Hj::Vmo::create(Hj::ROOT, bootfsRecord->start, bootfsRecord->size, Hj::VmoFlags::DMA));
    co_try$(vmo.label("bootfs"));

    auto fd = makeRc<Sys::Skift::VmoFd>(std::move(vmo), 0);
    auto root = co_trya$(Karm::Fs::mountBootfsAsync(fd));

    auto handler = makeRc<Strata::Fs::FsHandler>(root);
    auto server = co_trya$(Ipc::Server::createAsync("ipc://strata-fs"_url, handler));
    co_return co_await server.servAsync(ct);
}
