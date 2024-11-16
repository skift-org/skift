#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-logger/logger.h>

#include <karm-sys/_embed.h>

#include "fd.h"

namespace Karm::Sys::_Embed {

Res<Strong<Sys::Fd>> unpackFd(Io::PackScan &s) {
    return Skift::unpackFd(s);
}

// MARK: File I/O --------------------------------------------------------------

Res<Strong<Sys::Fd>> openFile(Mime::Url const &url) {
    auto urlStr = url.str();
    auto *fileRecord = useHandover().fileByName(urlStr.buf());
    if (not fileRecord)
        return Error::invalidFilename();
    auto vmo = try$(Hj::Vmo::create(Hj::ROOT, fileRecord->start, fileRecord->size, Hj::VmoFlags::DMA));
    try$(vmo.label(urlStr));

    return Ok(makeStrong<Skift::VmoFd>(std::move(vmo)));
}

Res<Strong<Sys::Fd>> createFile(Mime::Url const &) {
    notImplemented();
}

Res<Strong<Sys::Fd>> openOrCreateFile(Mime::Url const &) {
    notImplemented();
}

Res<Cons<Strong<Sys::Fd>, Strong<Sys::Fd>>> createPipe() {
    notImplemented();
}

Res<Strong<Sys::Fd>> createIn() {
    return Ok(makeStrong<Sys::NullFd>());
}

Res<Strong<Sys::Fd>> createOut() {
    return Ok(makeStrong<Sys::NullFd>());
}

Res<Strong<Sys::Fd>> createErr() {
    return Ok(makeStrong<Sys::NullFd>());
}

Res<Vec<Sys::DirEntry>> readDir(Mime::Url const &) {
    notImplemented();
}

Res<Stat> stat(Mime::Url const &) {
    notImplemented();
}

// MARK: User interactions -----------------------------------------------------

Res<> launch(Mime::Uti const &, Mime::Url const &) {
    notImplemented();
}

Async::Task<> launchAsync(Mime::Uti const &, Mime::Url const &) {
    notImplemented();
}

// MARK: Sockets ---------------------------------------------------------------

Res<Strong<Sys::Fd>> connectTcp(SocketAddr) {
    notImplemented();
}

Res<Strong<Sys::Fd>> listenTcp(SocketAddr) {
    notImplemented();
}

Res<Strong<Sys::Fd>> listenUdp(SocketAddr) {
    notImplemented();
}

Res<Strong<Sys::Fd>> listenIpc(Mime::Url) {
    notImplemented();
}

// MARK: Time ------------------------------------------------------------------

TimeStamp now() {
    return Hj::now().unwrap();
}

TimeSpan uptime() {
    notImplemented();
}

// MARK: Memory Managment ------------------------------------------------------

Res<Sys::MmapResult> memMap(Sys::MmapOptions const &) {
    notImplemented();
}

Res<Sys::MmapResult> memMap(Sys::MmapOptions const &, Strong<Sys::Fd> fd) {
    auto vmoFd = fd.is<Skift::VmoFd>();
    if (not vmoFd)
        return Error::invalidInput("expected VmoFd");

    auto &vmo = vmoFd->vmo();
    auto range = try$(Hj::Space::self().map(vmo, Hj::MapFlags::READ | Hj::MapFlags::WRITE));

    return Ok(Sys::MmapResult{
        0,
        range.start,
        range.size,
    });
}

Res<> memUnmap(void const *ptr, usize size) {
    return Hj::Space::self().unmap({(usize)ptr, size});
}

Res<> memFlush(void *, usize) {
    notImplemented();
}

// MARK: System Informations ---------------------------------------------------

Res<> populate(Sys::SysInfo &) {
    notImplemented();
}

Res<> populate(Sys::MemInfo &) {
    notImplemented();
}

Res<> populate(Vec<Sys::CpuInfo> &) {
    notImplemented();
}

Res<> populate(Sys::UserInfo &) {
    notImplemented();
}

Res<> populate(Vec<Sys::UserInfo> &) {
    notImplemented();
}

// MARK: Process Managment -----------------------------------------------------

Res<> sleep(TimeSpan span) {
    return sleepUntil(now() + span);
}

Res<> sleepUntil(TimeStamp until) {
    // HACK: listerner abuses ahead
    auto listener = try$(Hj::Listener::create(Hj::ROOT));
    try$(listener.poll(until));
    return Ok();
}

Res<> exit(i32) {
    notImplemented();
}

// MARK: Sandboxing ------------------------------------------------------------

void hardenSandbox() {
    logError("could not harden sandbox");
}

} // namespace Karm::Sys::_Embed
