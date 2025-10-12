module;

#include <handover/hook.h>
#include <hjert-api/api.h>

#include "fd.h"

module Karm.Sys;

import Karm.Ref;
import Karm.Logger;

namespace Karm::Sys::_Embed {

Res<Rc<Sys::Fd>> deserializeFd(Serde::Deserializer&) {
    return Error::notImplemented();
}

// MARK: File I/O --------------------------------------------------------------

Res<Rc<Fd>> openFile(Ref::Url const& url) {
    auto urlStr = url.str();
    auto* fileRecord = useHandover().fileByName(urlStr.buf());
    if (not fileRecord)
        return Error::invalidFilename();
    auto vmo = try$(Hj::Vmo::create(Hj::ROOT, fileRecord->start, fileRecord->size, Hj::VmoFlags::DMA));
    try$(vmo.label(urlStr));

    return Ok(makeRc<Skift::VmoFd>(std::move(vmo)));
}

Res<Rc<Fd>> createFile(Ref::Url const&) {
    return Error::notImplemented();
}

Res<Rc<Fd>> openOrCreateFile(Ref::Url const&) {
    return Error::notImplemented();
}

Res<Pair<Rc<Fd>, Rc<Fd>>> createPipe() {
    return Error::notImplemented();
}

Res<Rc<Fd>> createIn() {
    return Ok(makeRc<NullFd>());
}

Res<Rc<Fd>> createOut() {
    return Ok(makeRc<NullFd>());
}

Res<Rc<Fd>> createErr() {
    return Ok(makeRc<NullFd>());
}

Res<Vec<DirEntry>> readDir(Ref::Url const&) {
    return Error::notImplemented();
}

Res<> createDir(Ref::Url const&) {
    return Error::notImplemented();
}

Res<Vec<DirEntry>> readDirOrCreate(Ref::Url const&) {
    return Error::notImplemented();
}

Res<Stat> stat(Ref::Url const&) {
    return Error::notImplemented();
}

// MARK: User interactions -----------------------------------------------------

Res<> launch(Intent) {
    return Error::notImplemented();
}

[[clang::coro_wrapper]]
Async::Task<> launchAsync(Intent) {
    notImplemented();
}

// MARK: Process ---------------------------------------------------------------

Res<Rc<Pid>> run(Command const&) {
    notImplemented();
}

// MARK: Sockets ---------------------------------------------------------------

Res<Rc<Fd>> connectTcp(SocketAddr) {
    return Error::notImplemented();
}

Res<Rc<Fd>> listenTcp(SocketAddr) {
    return Error::notImplemented();
}

Res<Rc<Fd>> listenUdp(SocketAddr) {
    return Error::notImplemented();
}

Res<Rc<Fd>> listenIpc(Ref::Url) {
    return Error::notImplemented();
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

Res<MmapResult> memMap(Sys::MmapProps const&) {
    return Error::notImplemented();
}

Res<MmapResult> memMap(Sys::MmapProps const&, Rc<Fd> fd) {
    auto vmoFd = fd.is<Skift::VmoFd>();
    if (not vmoFd)
        return Error::invalidInput("expected VmoFd");

    auto& vmo = vmoFd->vmo();
    auto range = try$(Hj::Space::self().map(vmo, {Hj::MapFlags::READ, Hj::MapFlags::WRITE}));

    return Ok(MmapResult{
        0,
        range.start,
        range.size,
    });
}

Res<> memUnmap(void const* ptr, usize size) {
    return Hj::Space::self().unmap({(usize)ptr, size});
}

Res<> memFlush(void*, usize) {
    return Error::notImplemented();
}

// MARK: System Informations ---------------------------------------------------

Res<> populate(SysInfo&) {
    return Error::notImplemented();
}

Res<> populate(MemInfo&) {
    return Error::notImplemented();
}

Res<> populate(Vec<CpuInfo>&) {
    return Error::notImplemented();
}

Res<> populate(UserInfo&) {
    return Error::notImplemented();
}

Res<> populate(Vec<UserInfo>&) {
    return Error::notImplemented();
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
    return Hj::Task::self().ret();
}

Res<Ref::Url> pwd() {
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

// MARK: Bundle ----------------------------------------------------------------

Res<Vec<String>> installedBundles() {
    return Error::notImplemented("not implemented");
}

Res<String> currentBundle() {
    return Error::notImplemented("not implemented");
}

} // namespace Karm::Sys::_Embed
