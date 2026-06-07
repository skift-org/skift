module;

#include <hal/mem.h>
#include <karm/macros>

module Karm.Sys;

import Karm.Ref;
import Karm.Logger;
import Karm.Sys.Skift;
import Karm.Ipc;
import Strata.Protos;
import Hjert.Api;

namespace Karm::Sys::_Embed {

Res<Rc<Fd>> deserializeFd(Serde::Deserializer& de) {
    auto scope = try$(de.beginScope({.kind = Serde::Type::OBJECT}));
    auto [_, type] = try$(scope.deserializeUnit<Skift::FdType>({.kind = Serde::Type::OBJECT_ITEM}));
    Res<Rc<Fd>> result = Error::invalidData("unknow fd type");

    if (type == Skift::FdType::VMO) {
        auto [_, vmo] = try$(scope.deserializeUnit<Hj::Vmo>({.kind = Serde::Type::OBJECT_ITEM}));
        result = Ok(makeRc<Skift::VmoFd>(std::move(vmo), 0));
    } else if (type == Skift::FdType::DUPLEX) {
        auto [_, in] = try$(scope.deserializeUnit<Hj::Channel>({.kind = Serde::Type::OBJECT_ITEM}));
        auto [_, out] = try$(scope.deserializeUnit<Hj::Channel>({.kind = Serde::Type::OBJECT_ITEM}));
        result = Ok(makeRc<Skift::ChannelFd>(std::move(in), std::move(out)));
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
    } else if (url.scheme == "location") {
        auto path = url.path;
        path.rooted = false;

        if (url.host == "home")
            return Ok("/home"_path / path);

        return Ok("/home"_path / try$(Io::toPascalCase(url.host.str())) / path);
    } else if (url.scheme == "bundle") {
        auto path = url.path;
        path.rooted = false;
        return Ok("/bundles"_path / url.host.str() / path);
    } else {
        logError("unsupported scheme: {}", url.scheme);
        return Error::notImplemented();
    }
}

Res<Rc<Fd>> openFile(Ref::Url const& url, Flags<OpenOption>) {
    auto path = try$(_resolveUrl(url));
    auto file = try$(Async::run(Ipc::Client::connectAsync("file:"_url / path, Async::CancellationToken::uninterruptible())));
    return Ok(makeRc<Skift::FsFd>(std::move(file)));
}

Res<Pair<Rc<Fd>, Rc<Fd>>> createPipe() {
    return Error::notImplemented("createPipe() not implemented");
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

Async::Task<Vec<DirEntry>> readDirAsync(Ref::Url const& url, Async::CancellationToken ct) {
    auto path = co_try$(_resolveUrl(url));
    auto dir = co_trya$(Ipc::Client::connectAsync("file:"_url / path, ct));
    auto result = co_trya$(dir.callAsync<Strata::IFs::ReadDir>({}, ct));
    co_return result;
}

Res<Vec<DirEntry>> readDir(Ref::Url const& url) {
    return Sys::run(readDirAsync(url, Async::CancellationToken::uninterruptible()));
}

Res<> createDir(Ref::Url const&) {
    return Error::notImplemented("createDir() not implemented");
}

Res<Vec<DirEntry>> readDirOrCreate(Ref::Url const&) {
    return Error::notImplemented("readDirOrCreate() not implemented");
}

Res<Stat> stat(Ref::Url const& url) {
    auto fd = try$(openFile(url, OpenOption::READ));
    return fd->stat();
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
    return Error::notImplemented("spawn() not implemented");
}

Res<Tuple<Rc<Pid>, Rc<Fd>>> spawnPty(Command const&) {
    return Error::notImplemented("spawnPty() not implemented");
}

// MARK: Sockets ---------------------------------------------------------------

Res<Rc<Fd>> connectTcp(SocketAddr) {
    return Error::notImplemented("connectTcp() not implemented");
}

Res<Rc<Fd>> listenTcp(SocketAddr) {
    return Error::notImplemented("listenTcp() not implemented");
}

Res<Rc<Fd>> listenUdp(SocketAddr) {
    return Error::notImplemented("listenUdp() not implemented");
}

Res<Rc<Fd>> connectIpc(Ref::Url url) {
    auto [chan0, chan1] = try$(Skift::ChannelFd::create(url.host.str()));
    try$(Skift::globalClient().notify(Strata::ICm::Connect{chan1, url}));
    return Ok(chan0);
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
    return Error::notImplemented("memMap() not implemented");
}

Res<MmapResult> memMap(MmapProps const& props, Rc<Fd> fd) {
    if (auto it = fd.is<Skift::FsFd>()) {
        auto vmoFd = try$(
            Sys::run(
                it->_client
                    .callAsync<Strata::IFs::Mmap>(
                        {},
                        Async::CancellationToken::uninterruptible()
                    )
            )
        );
        return memMap(props, vmoFd);
    } else if (auto it = fd.is<Skift::VmoFd>()) {
        auto& vmo = it->vmo();
        auto range = try$(Hj::Space::self().map(vmo, {Hj::MapFlags::READ, Hj::MapFlags::WRITE}));

        return Ok(MmapResult{
            0,
            range.start,
            range.size,
        });
    } else {
        return Error::unsupported("fd is not mappable");
    }
}

Res<> memUnmap(void const* ptr, usize size) {
    return Hj::Space::self().unmap({(usize)ptr, size});
}

Res<> memFlush(void*, usize) {
    return Error::notImplemented("memFlush() not implemented");
}

usize pageSize() {
    return Hal::PAGE_SIZE;
}

// MARK: System Informations ---------------------------------------------------

Res<> populate(SysInfo& infos) {
    infos.sysName = "skiftOS"s;
    infos.sysVersion = "0.1"s;
    infos.kernelName = "hjert"s;
    infos.kernelVersion = "0.1"s;
    infos.hostname = "localhost"s;
    return Ok();
}

Res<> populate(MemInfo&) {
    return Error::notImplemented("populate() not implemented");
}

Res<> populate(Vec<CpuInfo>&) {
    return Error::notImplemented("populate() not implemented");
}

Res<> populate(UserInfo& info) {
    info.name = "Root"s;
    info.shell = "bundle://luna"_url;
    info.home = "location://home"_url;
    return Ok();
}

Res<> populate(Vec<UserInfo>&) {
    return Error::notImplemented("populate() not implemented");
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
    co_return Error::notImplemented("ipLookupAsync() not implemented");
}

// MARK: Bundle ----------------------------------------------------------------

Res<Vec<String>> installedBundles() {
    return Error::notImplemented("installedBundles() not implemented");
}

Res<String> currentBundle() {
    return Error::notImplemented("currentBundle() not implemented");
}

} // namespace Karm::Sys::_Embed
