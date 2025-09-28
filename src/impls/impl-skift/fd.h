#pragma once

#include <hjert-api/api.h>
#include <karm-logger/logger.h>

import Karm.Sys;

template <Meta::Derive<Hj::Object> T>
struct Karm::Sys::MessagePacker<T> {
    static Res<> pack(Karm::Sys::MessageWriter& e, T const& val) {
        e.give(Sys::Handle{val.raw()});
        return Ok();
    }

    static Res<T> unpack(Karm::Sys::MessageReader& s) {
        auto cap = s.take();
        return Ok(T{Hj::Cap{cap.value()}});
    }
};

namespace Skift {

enum struct _FdType : u8 {
    NONE,
    VMO,
    IPC,
};

struct VmoFd : public Sys::NullFd {
    Hj::Vmo _vmo;

    Hj::Vmo& vmo() {
        return _vmo;
    }

    VmoFd(Hj::Vmo vmo)
        : _vmo(std::move(vmo)) {}

    Sys::Handle handle() const override {
        return Sys::Handle(_vmo.raw());
    }

    Res<> pack(Sys::MessageWriter& e) override {
        try$(Sys::pack(e, _FdType::VMO));
        try$(Sys::pack(e, _vmo));
        return Ok();
    }
};

struct IpcFd : public Sys::NullFd {
    Hj::Channel _in;
    Hj::Channel _out;

    IpcFd(Hj::Channel in, Hj::Channel out)
        : _in(std::move(in)), _out(std::move(out)) {}

    Sys::Handle handle() const override {
        return Sys::INVALID;
    }

    Res<Sys::_Sent> send(Bytes buf, Slice<Sys::Handle> hnds, Sys::SocketAddr) override {
        auto [bytes, caps] = try$(_out.send(buf, hnds.cast<Hj::Cap>()));
        return Ok<Sys::_Sent>(bytes, caps);
    }

    Res<Sys::_Received> recv(MutBytes buf, MutSlice<Sys::Handle> hnds) override {
        auto [bytes, caps] = try$(_in.recv(buf, hnds.cast<Hj::Cap>()));
        return Ok<Sys::_Received>(bytes, caps, Sys::Ip4::unspecified(0)); // FIXME: Placeholder address
    }

    Res<> pack(Sys::MessageWriter& w) override {
        try$(Sys::pack(w, _FdType::IPC));
        try$(Sys::pack(w, _in));
        try$(Sys::pack(w, _out));
        return Ok();
    }
};

Res<Rc<Sys::Fd>> unpackFd(Sys::MessageReader& s);

} // namespace Skift
