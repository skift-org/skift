module;

#include <hjert-api/api.h>

export module Karm.Sys.Skift:fd;

import Karm.Sys;
import Karm.Logger;

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

    Hj::Vmo& vmo() {
        return _vmo;
    }

    VmoFd(Hj::Vmo vmo)
        : _vmo(std::move(vmo)) {}

    Res<> serialize(Serde::Serializer& ser) const override {
        auto scope = try$(ser.beginScope({.kind = Serde::Type::OBJECT}));
        try$(scope.serializeUnit({.kind = Serde::Type::OBJECT_ITEM}, FdType::VMO));
        try$(scope.serializeUnit({.kind = Serde::Type::OBJECT_ITEM}, _vmo));
        return scope.end();
    }
};

export struct DuplexFd : NullFd {
    Hj::Channel _in;
    Hj::Channel _out;

    static Res<Rc<DuplexFd>> create(Str label) {
        auto in = try$(Hj::Channel::create(Hj::Domain::self(), kib(16), 16));
        try$(in.label(Io::format("{}-in", label)));

        auto out = try$(Hj::Channel::create(Hj::Domain::self(), kib(16), 16));
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
        auto pipe = try$(Hj::Pipe::create(Hj::ROOT, kib(16)));
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
