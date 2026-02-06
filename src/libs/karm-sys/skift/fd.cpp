module;

#include <karm/entry>

export module Karm.Sys.Skift:fd;

import Karm.Sys;
import Karm.Logger;
import Hjert.Api;

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
        if (not _mapped) {
            _mapped = Hj::map(_vmo, {Hj::MapFlags::READ}).take();
        }
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
        auto in = try$(Hj::Channel::create(Hj::Domain::self(), 16_KiB, 16));
        try$(in.label(Io::format("{}-in", label)));

        auto out = try$(Hj::Channel::create(Hj::Domain::self(), 16_KiB, 16));
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
        auto pipe = try$(Hj::Pipe::create(Hj::ROOT, 16_KiB));
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

export struct IpcListenerFd : NullFd {
};

} // namespace Karm::Sys::Skift
