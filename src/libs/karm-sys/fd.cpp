#include "fd.h"

namespace Karm::Sys {

Res<usize> NullFd::read(MutBytes) {
    return Ok(0uz);
}

Res<usize> NullFd::write(Bytes) {
    return Ok(0uz);
}

Res<usize> NullFd::seek(Io::Seek) {
    return Ok(0uz);
}

Res<usize> NullFd::flush() {
    return Ok(0uz);
}

Res<Strong<Fd>> NullFd::dup() {
    return Ok(makeStrong<NullFd>());
}

Res<_Accepted> NullFd::accept() {
    return Ok<_Accepted>(
        makeStrong<NullFd>(),
        Ip4::unspecified(0));
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

} // namespace Karm::Sys
