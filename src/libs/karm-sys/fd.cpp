#include "fd.h"

#include "_embed.h"

namespace Karm::Sys {

Res<Rc<Fd>> Fd::unpack(Io::PackScan& s) {
    return _Embed::unpackFd(s);
}

Res<usize> NullFd::read(MutBytes) {
    return Ok(0uz);
}

Res<usize> NullFd::write(Bytes) {
    return Ok(0uz);
}

Res<usize> NullFd::seek(Io::Seek) {
    return Ok(0uz);
}

Res<> NullFd::flush() {
    return Ok();
}

Res<Rc<Fd>> NullFd::dup() {
    return Ok(makeRc<NullFd>());
}

Res<_Accepted> NullFd::accept() {
    return Ok<_Accepted>(
        makeRc<NullFd>(),
        Ip4::unspecified(0)
    );
}

Res<Stat> NullFd::stat() {
    return Ok(Stat{});
}

Res<_Sent> NullFd::send(Bytes, Slice<Handle>, SocketAddr) {
    return Ok<_Sent>(0uz, 0uz);
}

Res<_Received> NullFd::recv(MutBytes, MutSlice<Handle>) {
    return Ok<_Received>(0uz, 0uz, Ip4::unspecified(0));
}

Res<> NullFd::pack(Io::PackEmit& e) {
    return Io::pack(e, INVALID);
}

} // namespace Karm::Sys
