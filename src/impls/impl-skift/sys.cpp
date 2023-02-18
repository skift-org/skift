#include <embed-sys/sys.h>

namespace Embed {

Res<Strong<Sys::Fd>> createIn() {
    return Ok(makeStrong<Sys::DummyFd>());
}

Res<Strong<Sys::Fd>> createOut() {
    return Ok(makeStrong<Sys::DummyFd>());
}

Res<Strong<Sys::Fd>> createErr() {
    return Ok(makeStrong<Sys::DummyFd>());
}

} // namespace Embed
