#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-logger/logger.h>
#include <karm-sys/_embed.h>
#include <karm-sys/launch.h>

#include "fd.h"

namespace Karm::Sys::_Embed {

Res<Rc<Sys::Fd>> unpackFd(Io::PackScan& s) {
    return Skift::unpackFd(s);
}

// MARK: File I/O --------------------------------------------------------------

Res<Rc<Sys::Fd>> openFile(Mime::Url const& url) {
    auto urlStr = url.str();
    auto* fileRecord = useHandover().fileByName(urlStr.buf());
    if (not fileRecord)
        return Error::invalidFilename();
    auto vmo = try$(Hj::Vmo::create(Hj::ROOT, fileRecord->start, fileRecord->size, Hj::VmoFlags::DMA));
    try$(vmo.label(urlStr));

    return Ok(makeRc<Skift::VmoFd>(std::move(vmo)));
}

Res<Rc<Sys::Fd>> createFile(Mime::Url const&) {
    notImplemented();
}

Res<Rc<Sys::Fd>> openOrCreateFile(Mime::Url const&) {
    notImplemented();
}

Res<Pair<Rc<Sys::Fd>, Rc<Sys::Fd>>> createPipe() {
    notImplemented();
}

Res<Rc<Sys::Fd>> createIn() {
    return Ok(makeRc<Sys::NullFd>());
}

Res<Rc<Sys::Fd>> createOut() {
    return Ok(makeRc<Sys::NullFd>());
}

Res<Rc<Sys::Fd>> createErr() {
    return Ok(makeRc<Sys::NullFd>());
}

Res<Vec<Sys::DirEntry>> readDir(Mime::Url const&) {
    notImplemented();
}

Res<Stat> stat(Mime::Url const&) {
    notImplemented();
}

// MARK: User interactions -----------------------------------------------------

Res<> launch(Intent) {
    notImplemented();
}

[[clang::coro_wrapper]]
Async::Task<> launchAsync(Intent) {
    notImplemented();
}

// MARK: Sockets ---------------------------------------------------------------

Res<Rc<Sys::Fd>> connectTcp(SocketAddr) {
    notImplemented();
}

Res<Rc<Sys::Fd>> listenTcp(SocketAddr) {
    notImplemented();
}

Res<Rc<Sys::Fd>> listenUdp(SocketAddr) {
    notImplemented();
}

Res<Rc<Sys::Fd>> listenIpc(Mime::Url) {
    notImplemented();
}

// MARK: Time ------------------------------------------------------------------

SystemTime now() {
    // TODO
    return SystemTime::epoch();
}

Instant instant() {
    // TODO
    return Hj::now().unwrap();
}

Duration uptime() {
    notImplemented();
}

// MARK: Memory Managment ------------------------------------------------------

Res<Sys::MmapResult> memMap(Sys::MmapOptions const&) {
    notImplemented();
}

Res<Sys::MmapResult> memMap(Sys::MmapOptions const&, Rc<Sys::Fd> fd) {
    auto vmoFd = fd.is<Skift::VmoFd>();
    if (not vmoFd)
        return Error::invalidInput("expected VmoFd");

    auto& vmo = vmoFd->vmo();
    auto range = try$(Hj::Space::self().map(vmo, Hj::MapFlags::READ | Hj::MapFlags::WRITE));

    return Ok(Sys::MmapResult{
        0,
        range.start,
        range.size,
    });
}

Res<> memUnmap(void const* ptr, usize size) {
    return Hj::Space::self().unmap({(usize)ptr, size});
}

Res<> memFlush(void*, usize) {
    notImplemented();
}

// MARK: System Informations ---------------------------------------------------

Res<> populate(Sys::SysInfo&) {
    notImplemented();
}

Res<> populate(Sys::MemInfo&) {
    notImplemented();
}

Res<> populate(Vec<Sys::CpuInfo>&) {
    notImplemented();
}

Res<> populate(Sys::UserInfo&) {
    notImplemented();
}

Res<> populate(Vec<Sys::UserInfo>&) {
    notImplemented();
}

// MARK: Process Managment -----------------------------------------------------

Res<> sleep(Duration span) {
    return sleepUntil(instant() + span);
}

Res<> sleepUntil(Instant until) {
    // HACK: listerner abuses ahead
    auto listener = try$(Hj::Listener::create(Hj::ROOT));
    try$(listener.poll(until));
    return Ok();
}

Res<> exit(i32) {
    notImplemented();
}

Res<Mime::Url> pwd() {
    return Ok("file:/"_url);
}

// MARK: Sandboxing ------------------------------------------------------------

Res<> hardenSandbox() {
    // TODO: Implement sandboxing
    return Ok();
}

// MARK: Addr ------------------------------------------------------------------

Async::Task<Vec<Ip>> ipLookupAsync(Str) {
    co_return Error::notImplemented();
}

} // namespace Karm::Sys::_Embed
