#include <karm-sys/entry.h>
#include <vaerk-handover/hook.h>
#include <hjert-api/api.h>


import Strata.Protos;
import Karm.Sys.Skift;
import Karm.Fs;

using namespace Karm;

namespace Strata::Fs {


struct Server {
    Rc<Karm::Fs::Node> _root;
    Map<IFs::Fid, Rc<Karm::Fs::Node>> _files = {};
    IFs::Fid _fids = 1;

    static Async::Task<Server> createAsync(Sys::Context& ctx, Async::CancellationToken ) {
        auto& handover = useHandover(ctx);

        auto* bootfsRecord = handover.fileByName("file:/skift/init.bootfs");
        if (not bootfsRecord)
            co_return Error::notFound("no bootfs");

        auto vmo = co_try$(Hj::Vmo::create(Hj::ROOT, bootfsRecord->start, bootfsRecord->size, Hj::VmoFlags::DMA));
        co_try$(vmo.label("bootfs"));

        auto fd = makeRc<Sys::Skift::VmoFd>(std::move(vmo));
        co_return co_await Karm::Fs::mountBootfsAsync(fd);
    }

    Async::Task<IFs::Open::Response> _handleOpenAsync(Sys::Message& message) {
        auto request = co_try$(message.unpack<IFs::Open>());
        auto file = co_trya$(_root->lookupAsync(Ref::Path{true, request.path}));
        auto fid = _fids++;
        _files.put(fid, file);
        co_return Ok(fid);
    }

    Async::Task<IFs::Close::Response> _handleCloseAsync(Sys::Message& message) {
        auto request = co_try$(message.unpack<IFs::Close>());
        if (not _files.has(request.fid))
            co_return Error::invalidInput();
        _files.del(request.fid);
        co_return Ok();
    }

    Res<Rc<Karm::Fs::Node>> _resolveFid(IFs::Fid fid) {
        return _files.tryGet(fid).okOr(Error::invalidInput());
    }

    Async::Task<IFs::Read::Response> _handleReadAsync(Sys::Message& message) {
        auto request = co_try$(message.unpack<IFs::Read>());
        auto node = co_try$(_resolveFid(request.fid));
        Vec<u8> buf;
        buf.resize(request.len);
        co_trya$(node->readAsync(buf, request.off));
        co_return Ok(std::move(buf));
    }

    Async::Task<IFs::Write::Response> _handleWriteAsync(Sys::Message& message) {
        auto request = co_try$(message.unpack<IFs::Write>());
        auto node = co_try$(_resolveFid(request.fid));
        co_return co_await node->writeAsync(request.buf, request.off);
    }

    Async::Task<IFs::ReadDir::Response> _handleReadDirAsync(Sys::Message& message) {
        auto request = co_try$(message.unpack<IFs::ReadDir>());
        auto node = co_try$(_resolveFid(request.fid));
        co_return co_await node->listAsync();
    }

    Async::Task<IFs::Stat::Response> _handleStatAsync(Sys::Message& message) {
        auto request = co_try$(message.unpack<IFs::Write>());
        auto node = co_try$(_resolveFid(request.fid));
        co_return co_await node->statAsync();
    }

    Async::Task<> servAsync(Sys::Context& ctx, Async::CancellationToken ct) {
        auto endpoint = Sys::Endpoint::adopt(ctx);

        while (true) {
            auto msg = co_trya$(endpoint.recvAsync(ct));

            if (msg.is<IFs::Open>()) {
                (void)endpoint.resp<IFs::Open>(msg, co_await _handleOpenAsync(msg));
            } else if (msg.is<IFs::Close>()) {
                (void)endpoint.resp<IFs::Close>(msg, co_await _handleCloseAsync(msg));
            } else if (msg.is<IFs::Read>()) {
                (void)endpoint.resp<IFs::Read>(msg, co_await _handleReadAsync(msg));
            } else if (msg.is<IFs::Write>()) {
                (void)endpoint.resp<IFs::Write>(msg, co_await _handleWriteAsync(msg));
            } else if (msg.is<IFs::ReadDir>()) {
                (void)endpoint.resp<IFs::ReadDir>(msg, co_await _handleReadDirAsync(msg));
            } else if (msg.is<IFs::Stat>()) {
                (void)endpoint.resp<IFs::Stat>(msg, co_await _handleStatAsync(msg));
            }
        }
    }
};

} // namespace Strata::Fs

Async::Task<> entryPointAsync(Sys::Context& ctx, Async::CancellationToken ct) {
    auto server = co_trya$(Strata::Fs::Server::createAsync(ctx, ct));
    co_return co_await server.servAsync(ctx, ct);
}