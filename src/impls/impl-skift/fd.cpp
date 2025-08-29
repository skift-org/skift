#include "fd.h"

namespace Skift {

Res<Rc<Sys::Fd>> unpackFd(Sys::MessageReader& s) {
    auto type = try$(Sys::unpack<_FdType>(s));
    switch (type) {
    case _FdType::VMO: {
        auto vmo = try$(Sys::unpack<Hj::Vmo>(s));
        return Ok(makeRc<VmoFd>(std::move(vmo)));
    }

    case _FdType::IPC: {
        auto in = try$(Sys::unpack<Hj::Channel>(s));
        auto out = try$(Sys::unpack<Hj::Channel>(s));
        return Ok(makeRc<IpcFd>(std::move(in), std::move(out)));
    }

    default:
    case _FdType::NONE:
        return Ok(makeRc<Sys::NullFd>());
    }
}

} // namespace Skift
