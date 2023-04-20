#include <embed-sys/sys.h>

namespace Embed {

/* --- File I/O ------------------------------------------------------------- */

Res<Strong<Sys::Fd>> openFile(Sys::Url) {
    panic("not implemented");
}

Res<Strong<Sys::Fd>> createFile(Sys::Url) {
    panic("not implemented");
}

Res<Strong<Sys::Fd>> createIn() {
    return Ok(makeStrong<Sys::DummyFd>());
}

Res<Strong<Sys::Fd>> createOut() {
    return Ok(makeStrong<Sys::DummyFd>());
}

Res<Strong<Sys::Fd>> createErr() {
    return Ok(makeStrong<Sys::DummyFd>());
}

/* --- Time ----------------------------------------------------------------- */

TimeStamp now() {
    panic("not implemented");
}

/* --- Memory Managment ----------------------------------------------------- */

Res<Sys::MmapResult> memMap(Sys::MmapOptions const &, Strong<Sys::Fd>) {
    panic("not implemented");
}

Res<> memUnmap(void const *, usize) {
    panic("not implemented");
}

} // namespace Embed
