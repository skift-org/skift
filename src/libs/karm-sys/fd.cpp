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

Res<Cons<Strong<Fd>, SocketAddr>> NullFd::accept() {
    return Ok<Cons<Strong<Fd>, SocketAddr>>(
        makeStrong<NullFd>(),
        Ip4::unspecified(0));
}

Res<Stat> NullFd::stat() {
    return Ok(Stat{});
}

Res<> NullFd::sendFd(Strong<Fd>) {
    return Ok();
}

Res<Strong<Fd>> NullFd::recvFd() {
    return Ok(makeStrong<NullFd>());
}

Res<usize> NullFd::sendTo(Bytes, SocketAddr) {
    return Ok(0uz);
}

Res<Cons<usize, SocketAddr>> NullFd::recvFrom(MutBytes) {
    return Ok<Cons<usize, SocketAddr>>(0uz, Ip4::unspecified(0));
}

} // namespace Karm::Sys
