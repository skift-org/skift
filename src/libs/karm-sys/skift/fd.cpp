module;

#include <karm/entry>

export module Karm.Sys.Skift:fd;

import Hjert.Api;
import Karm.Logger;
import Karm.Sys;
import Strata.Protos;

import :client;

using namespace Karm;
using namespace Karm::Literals;

namespace Karm::Sys::Skift {

export enum struct FdType {
    VMO,
    DUPLEX,
    PIPE,

    _LEN,
};

export struct VmoFd : NullFd {
    Hj::Vmo _vmo;
    Opt<Hj::Mapped> _mapped;
    usize _off = 0;
    usize _len = 0;

    Hj::Vmo& vmo() {
        return _vmo;
    }

    VmoFd(Hj::Vmo vmo, usize len)
        : _vmo(std::move(vmo)), _len(len) {}

    Res<> serialize(Serde::Serializer& ser) const override {
        auto scope = try$(ser.beginScope({.kind = Serde::Type::OBJECT}));
        try$(scope.serializeUnit({.kind = Serde::Type::OBJECT_ITEM}, FdType::VMO));
        try$(scope.serializeUnit({.kind = Serde::Type::OBJECT_ITEM}, _vmo));
        return scope.end();
    }

    Hj::Mapped& _ensureMapped() {
        if (not _mapped)
            _mapped = Hj::map(_vmo, {Hj::MapFlags::READ}).take();
        return _mapped.unwrap();
    }

    Res<usize> read(MutBytes buf) override {
        auto read = copy(sub(_ensureMapped().bytes(), _off, _len), buf);
        _off += read;
        return Ok(read);
    }

    Res<usize> seek(Io::Seek s) override {
        _off = try$(s.apply(_off, _len));
        return Ok(_off);
    }

    Res<Rc<Fd>> slice(urange range) override {
        auto vmo = try$(Hj::Vmo::create(Hj::ROOT, range.start, range.size, {}, _vmo.cap()));
        return Ok(makeRc<VmoFd>(std::move(vmo), range.size));
    }
};

export struct ChannelFd : NullFd {
    Hj::Channel _in;
    Hj::Channel _out;

    static Res<Pair<Rc<ChannelFd>, Rc<ChannelFd>>> create(Str label) {
        auto [in0, out0] = try$(Hj::Channel::create(Hj::Domain::self(), 64_KiB, 128));
        try$(in0.label(Io::format("{}-in0", label)));
        try$(out0.label(Io::format("{}-out0", label)));

        auto [in1, out1] = try$(Hj::Channel::create(Hj::Domain::self(), 64_KiB, 128));
        try$(in1.label(Io::format("{}-in1", label)));
        try$(out1.label(Io::format("{}-out1", label)));

        return Ok(Pair{
            makeRc<ChannelFd>(std::move(in0), std::move(out1)),
            makeRc<ChannelFd>(std::move(in1), std::move(out0)),
        });
    }

    ChannelFd(Hj::Channel in, Hj::Channel out)
        : _in(std::move(in)), _out(std::move(out)) {}

    Res<_Sent> send(Bytes buf, Slice<Handle> hnds, SocketAddr) override {
        auto [bytes, caps] = try$(_out.send(buf, hnds.cast<Hj::Cap>()));
        return Ok<_Sent>(bytes, caps);
    }

    Res<_Received> recv(MutBytes buf, MutSlice<Handle> hnds) override {
        auto [bytes, caps] = try$(_in.recv(buf, hnds.cast<Hj::Cap>()));
        return Ok<_Received>(bytes, caps, Ip4::unspecified(0)); // FIXME: Placeholder address
    }

    Res<> serialize(Serde::Serializer& ser) const override {
        auto scope = try$(ser.beginScope({.kind = Serde::Type::OBJECT}));
        try$(scope.serializeUnit({.kind = Serde::Type::OBJECT_ITEM}, FdType::DUPLEX));
        try$(scope.serializeUnit({.kind = Serde::Type::OBJECT_ITEM}, _in));
        try$(scope.serializeUnit({.kind = Serde::Type::OBJECT_ITEM}, _out));
        return scope.end();
    }
};

export struct PipeFd : NullFd {
    Hj::Pipe _pipe;

    static Res<Rc<PipeFd>> create() {
        auto pipe = try$(Hj::Pipe::create(Hj::ROOT, 64_KiB));
        return Ok(makeRc<PipeFd>(std::move(pipe)));
    }

    explicit PipeFd(Hj::Pipe pipe)
        : _pipe(std::move(pipe)) {}

    Res<> serialize(Serde::Serializer& ser) const override {
        auto scope = try$(ser.beginScope({.kind = Serde::Type::OBJECT}));
        try$(scope.serializeUnit({.kind = Serde::Type::OBJECT_ITEM}, FdType::PIPE));
        try$(scope.serializeUnit({.kind = Serde::Type::OBJECT_ITEM}, _pipe));
        return scope.end();
    }
};

export struct FsFd : Fd {
    Ipc::Client _client;
    usize _off = 0;

    FsFd(Ipc::Client client) : _client(std::move(client)) {}

    Async::Task<usize> readAsync(MutBytes buf, Async::CancellationToken ct) {
        usize total = 0;

        while (total < buf.len()) {
            auto chunk = min(buf.len() - total, Ipc::MAX_TRANSFER_SIZE);
            auto response = co_trya$(
                _client.callAsync(
                    Strata::IFs::Read{
                        _off,
                        chunk,
                    },
                    ct
                )
            );
            auto read = copy(bytes(response.buf), mutSub(buf, total, total + chunk));
            _off += read;
            total += read;

            if (read < chunk)
                break;
        }

        co_return Ok(total);
    }

    Res<usize> read(MutBytes buf) override {
        return Sys::run(
            readAsync(buf, Async::CancellationToken::uninterruptible())
        );
    }

    Async::Task<usize> writeAsync(Bytes buf, Async::CancellationToken ct) {
        usize total = 0;
        while (total < buf.len()) {
            auto chunk = min(buf.len() - total, Ipc::MAX_TRANSFER_SIZE);
            auto written = co_trya$(
                _client.callAsync(
                    Strata::IFs::Write{
                        _off,
                        sub(buf, total, total + chunk),
                    },
                    ct
                )
            );
            _off += written;
            total += written;

            if (written < chunk)
                break;
        }

        co_return Ok(total);
    }

    Res<usize> write(Bytes buf) override {
        return Sys::run(
            writeAsync(buf, Async::CancellationToken::uninterruptible())
        );
    }

    Res<usize> seek(Io::Seek seek) override {
        auto size = try$(stat()).size;
        _off = try$(seek.apply(_off, size));
        return Ok(_off);
    }

    Res<> truncate(usize) override {
        return Error::unsupported();
    }

    Async::Task<> flushAsync(Async::CancellationToken) {
        co_return Ok();
    }

    Res<> flush() override {
        return Ok();
    }

    Res<Rc<Fd>> dup() override {
        return Error::unsupported();
    }

    Res<_Accepted> accept() override {
        return Error::unsupported();
    }

    Async::Task<Stat> statAsync(Async::CancellationToken ct) {
        co_return co_await _client.callAsync<Strata::IFs::Stat>({}, ct);
    }

    Res<Stat> stat() override {
        return Sys::run(
            statAsync(Async::CancellationToken::uninterruptible())
        );
    }

    Res<_Sent> send(Bytes, Slice<Handle>, SocketAddr) override {
        return Error::unsupported();
    }

    Res<_Received> recv(MutBytes, MutSlice<Handle>) override {
        return Error::unsupported();
    }

    Res<> serialize(Serde::Serializer&) const override {
        return Error::unsupported();
    }
};

export struct IpcListenerFd : NullFd {
};

} // namespace Karm::Sys::Skift
