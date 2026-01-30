module;

#include <hjert-api/api.h>
#include <vaerk-handover/hook.h>

module Karm.Sys;

import Karm.Ref;
import Karm.Logger;
import Karm.Sys.Skift;
import Strata.Protos;

namespace Karm::Sys::_Embed {

Res<Rc<Fd>> deserializeFd(Serde::Deserializer& de) {
    auto scope = try$(de.beginScope({.kind = Serde::Type::OBJECT}));
    auto [_, type] = try$(scope.deserializeUnit<Skift::FdType>({.kind = Serde::Type::OBJECT_ITEM}));
    Res<Rc<Fd>> result = Error::invalidData("unknow fd type");

    if (type == Skift::FdType::VMO) {
        auto [_, vmo] = try$(scope.deserializeUnit<Hj::Vmo>({.kind = Serde::Type::OBJECT_ITEM}));
        result = Ok(makeRc<Skift::VmoFd>(std::move(vmo)));
    } else if (type == Skift::FdType::DUPLEX) {
        auto [_, in] = try$(scope.deserializeUnit<Hj::Channel>({.kind = Serde::Type::OBJECT_ITEM}));
        auto [_, out] = try$(scope.deserializeUnit<Hj::Channel>({.kind = Serde::Type::OBJECT_ITEM}));
        result = Ok(makeRc<Skift::DuplexFd>(std::move(in), std::move(out)));
    } else if (type == Skift::FdType::PIPE) {
        auto [_, pipe] = try$(scope.deserializeUnit<Hj::Pipe>({.kind = Serde::Type::OBJECT_ITEM}));
        result = Ok(makeRc<Skift::PipeFd>(std::move(pipe)));
    }

    try$(scope.end());
    return result;
}

// MARK: File I/O --------------------------------------------------------------

static Res<Ref::Path> _resolveUrl(Ref::Url const& url) {
    if (url.scheme == "file") {
        return Ok(url.path);
    } else if (url.scheme == "bundle") {
        auto path = url.path;
        path.rooted = false;
        return Ok("/bundles"_path / url.host.str() / path);
    } else {
        logError("unsupported scheme: {}", url.scheme);
        return Error::notImplemented();
    }
}

Res<Rc<Fd>> openFile(Ref::Url const& url) {
    Ref::Path path = try$(_resolveUrl(url));
    path.rooted = false;
    auto bootfs = try$(Skift::Bootfs::ensure());
    auto vmo = try$(bootfs->openVmo(path.str()));
    return Ok(makeRc<Skift::VmoFd>(std::move(vmo)));
}

Res<Rc<Fd>> createFile(Ref::Url const& url) {
    return openFile(url);
}

Res<Rc<Fd>> openOrCreateFile(Ref::Url const& url) {
    return openFile(url);
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

Res<Stat> stat(Ref::Url const& url) {
    Ref::Path path = try$(_resolveUrl(url));
    path.rooted = false;
    auto bootfs = try$(Skift::Bootfs::ensure());
    auto dirent = try$(bootfs->openDirent(path.str()));
    return Ok(Stat{
        .type = Type::FILE,
        .size = dirent->length,
        .accessTime = SystemTime::epoch(),
        .modifyTime = SystemTime::epoch(),
        .changeTime = SystemTime::epoch(),
    });
}

// MARK: User interactions -----------------------------------------------------

Res<> launch(Intent intent) {
    auto url = try$(intent.handler.okOr(Error::invalidInput("no handler for intent")));
    return Skift::globalClient().notify(Strata::ICm::Launch(url));
}

[[clang::coro_wrapper]]
Async::Task<> launchAsync(Intent intent) {
    co_return _Embed::launch(intent);
}

// MARK: Process ---------------------------------------------------------------

Res<Rc<Pid>> spawn(Command const&) {
    notImplemented();
}

Res<Tuple<Rc<Pid>, Rc<Fd>>> spawnPty(Command const&) {
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

Res<Rc<Fd>> connectIpc(Ref::Url url) {
    auto fd = try$(Skift::DuplexFd::create(url.host.str()));
    try$(Skift::globalClient().notify(Strata::ICm::Connect{try$(fd->swap()), url}));
    return Ok(fd);
}

Res<Rc<Fd>> listenIpc(Ref::Url) {
    return Ok(makeRc<Skift::IpcListenerFd>());
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

Res<MmapResult> memMap(MmapProps const&) {
    return Error::notImplemented();
}

Res<MmapResult> memMap(MmapProps const&, Rc<Fd> fd) {
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

usize pageSize() {
    return Hal::PAGE_SIZE;
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
