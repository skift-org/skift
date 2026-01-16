module;

#include <hjert-api/api.h>

export module Karm.Sys.Skift:fd;

import Karm.Sys;
import Karm.Logger;

using namespace Karm;

namespace Karm::Sys::Skift {

export struct VmoFd : Sys::NullFd {
    Hj::Vmo _vmo;

    Hj::Vmo& vmo() {
        return _vmo;
    }

    VmoFd(Hj::Vmo vmo)
        : _vmo(std::move(vmo)) {}
};

export struct IpcFd : Sys::NullFd {
    Hj::Channel _in;
    Hj::Channel _out;

    IpcFd(Hj::Channel in, Hj::Channel out)
        : _in(std::move(in)), _out(std::move(out)) {}

    Res<Sys::_Sent> send(Bytes buf, Slice<Sys::Handle> hnds, Sys::SocketAddr) override {
        auto [bytes, caps] = try$(_out.send(buf, hnds.cast<Hj::Cap>()));
        return Ok<Sys::_Sent>(bytes, caps);
    }

    Res<Sys::_Received> recv(MutBytes buf, MutSlice<Sys::Handle> hnds) override {
        auto [bytes, caps] = try$(_in.recv(buf, hnds.cast<Hj::Cap>()));
        return Ok<Sys::_Received>(bytes, caps, Sys::Ip4::unspecified(0)); // FIXME: Placeholder address
    }
};

} // namespace Karm::Sys::Skift
