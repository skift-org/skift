#include "fd.h"

namespace Karm::Sys {

Res<usize> DummyFd::read(MutBytes) {
    return Ok(0uz);
}

Res<usize> DummyFd::write(Bytes) {
    return Ok(0uz);
}

Res<usize> DummyFd::seek(Io::Seek) {
    return Ok(0uz);
}

Res<usize> DummyFd::flush() {
    return Ok(0uz);
}

Res<Strong<Fd>> DummyFd::dup() {
    return Ok(makeStrong<DummyFd>());
}

Res<Cons<Strong<Fd>, SocketAddr>> DummyFd::accept() {
    return Ok<Cons<Strong<Fd>, SocketAddr>>(
        makeStrong<DummyFd>(),
        Ip4::unspecified(0));
}

Res<Stat> DummyFd::stat() {
    return Ok(Stat{});
}

} // namespace Karm::Sys
