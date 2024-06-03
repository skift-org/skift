#include "fd.h"

namespace Skift {

Res<Strong<Sys::Fd>> unpackFd(Io::PackScan &s) {
    auto type = try$(Io::unpack<_FdType>(s));
    switch (type) {
    case _FdType::VMO: {
        auto vmo = try$(Io::unpack<Hj::Vmo>(s));
        return Ok(makeStrong<VmoFd>(std::move(vmo)));
    }

    case _FdType::IPC: {
        auto in = try$(Io::unpack<Hj::Channel>(s));
        auto out = try$(Io::unpack<Hj::Channel>(s));
        return Ok(makeStrong<IpcFd>(std::move(in), std::move(out)));
    }

    default:
    case _FdType::NONE:
        return Ok(makeStrong<Sys::NullFd>());
    }
}

} // namespace Skift
