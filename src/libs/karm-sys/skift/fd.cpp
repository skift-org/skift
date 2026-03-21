module;

#include <karm/entry>

export module Karm.Sys.Skift:fd;

import Karm.Sys;
import Karm.Logger;
import Hjert.Api;
import Strata.Protos;
import :client;

using namespace Karm;

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
};

export struct DuplexFd : NullFd {
    Hj::Channel _in;
    Hj::Channel _out;

    static Res<Rc<DuplexFd>> create(Str label) {
        auto in = try$(Hj::Channel::create(Hj::Domain::self(), 64_KiB, 128));
        try$(in.label(Io::format("{}-in", label)));

        auto out = try$(Hj::Channel::create(Hj::Domain::self(), 64_KiB, 128));
        try$(out.label(Io::format("{}-out", label)));

        return Ok(makeRc<DuplexFd>(std::move(in), std::move(out)));
    }

    DuplexFd(Hj::Channel in, Hj::Channel out)
        : _in(std::move(in)), _out(std::move(out)) {}

    Res<_Sent> send(Bytes buf, Slice<Handle> hnds, SocketAddr) override {
        auto [bytes, caps] = try$(_out.send(buf, hnds.cast<Hj::Cap>()));
        return Ok<_Sent>(bytes, caps);
    }

    Res<_Received> recv(MutBytes buf, MutSlice<Handle> hnds) override {
        auto [bytes, caps] = try$(_in.recv(buf, hnds.cast<Hj::Cap>()));
        return Ok<_Received>(bytes, caps, Ip4::unspecified(0)); // FIXME: Placeholder address
    }

    Res<Rc<DuplexFd>> swap() {
        return Ok(
            makeRc<DuplexFd>(
                try$(_out.dup(Hj::ROOT)),
                try$(_in.dup(Hj::ROOT))
            )
        );
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
    Strata::IFs::Fid _fid;
    usize _off = 0;

    FsFd(Strata::IFs::Fid fid) : _fid(fid) {}

    ~FsFd() {
        (void)Sys::run(
            globalFsClient().callAsync<Strata::IFs::Close>({_fid}, Async::CancellationToken::uninterruptible())
        );
    }

    Async::Task<usize> readAsync(MutBytes buf, Async::CancellationToken ct) {
        auto response = co_trya$(globalFsClient().callAsync<Strata::IFs::Read>({_fid, _off, buf.len()}, ct));
        _off += copy(bytes(response.buf), buf);
        co_return Ok(response.buf.len());
    }

    Res<usize> read(MutBytes buf) override {
        return Sys::run(
            readAsync(buf, Async::CancellationToken::uninterruptible())
        );
    }

    Async::Task<usize> writeAsync(Bytes buf, Async::CancellationToken ct) {
        auto response = co_trya$(globalFsClient().callAsync<Strata::IFs::Write>({_fid, _off, buf}, ct));
        _off += response;
        co_return Ok(response);
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
        co_return co_await globalFsClient().callAsync(Strata::IFs::Stat{_fid}, ct);
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
