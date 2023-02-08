#include <embed-sys/sys.h>

namespace Embed {

Result<Strong<Sys::Fd>> createIn() {
    return {makeStrong<Sys::DummyFd>()};
}

Result<Strong<Sys::Fd>> createOut() {
    return {makeStrong<Sys::DummyFd>()};
}

Result<Strong<Sys::Fd>> createErr() {
    return {makeStrong<Sys::DummyFd>()};
}

} // namespace Embed
