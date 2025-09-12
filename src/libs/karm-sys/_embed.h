#pragma once

import Karm.Core;
import Karm.Ref;

#include "addr.h"
#include "async.h"
#include "dir.h"
#include "fd.h"
#include "info.h"
#include "types.h"

namespace Karm::Sys {

struct Intent;

} // namespace Karm::Sys

namespace Karm::Sys::_Embed {

// MARK: Fd --------------------------------------------------------------------

Res<Rc<Sys::Fd>> unpackFd(MessageReader&);

// MARK: File I/O --------------------------------------------------------------

Res<Rc<Sys::Fd>> openFile(Ref::Url const& url);

Res<Rc<Sys::Fd>> createFile(Ref::Url const& url);

Res<Rc<Sys::Fd>> openOrCreateFile(Ref::Url const& url);

Res<Pair<Rc<Sys::Fd>, Rc<Sys::Fd>>> createPipe();

Res<Rc<Sys::Fd>> createIn();

Res<Rc<Sys::Fd>> createOut();

Res<Rc<Sys::Fd>> createErr();

Res<Vec<Sys::DirEntry>> readDir(Ref::Url const& url);

Res<> createDir(Ref::Url const& url);

Res<Vec<Sys::DirEntry>> readDirOrCreate(Ref::Url const& url);

Res<Stat> stat(Ref::Url const& url);

// MARK: User interactions -----------------------------------------------------

Res<> launch(Intent intent);

Async::Task<> launchAsync(Intent intent);

// MARK: Sockets ---------------------------------------------------------------

Res<Rc<Sys::Fd>> listenUdp(SocketAddr addr);

Res<Rc<Sys::Fd>> connectTcp(SocketAddr addr);

Res<Rc<Sys::Fd>> listenTcp(SocketAddr addr);

Res<Rc<Sys::Fd>> listenIpc(Ref::Url url);

// MARK: Time ------------------------------------------------------------------

SystemTime now();

Instant instant();

Duration uptime();

// MARK: Memory Managment ------------------------------------------------------

Res<Sys::MmapResult> memMap(Sys::MmapProps const& options);

Res<Sys::MmapResult> memMap(Sys::MmapProps const& options, Rc<Sys::Fd> fd);

Res<> memUnmap(void const* buf, usize len);

Res<> memFlush(void* flush, usize len);

// MARK: System Informations ---------------------------------------------------

Res<> populate(Sys::SysInfo&);

Res<> populate(Sys::MemInfo&);

Res<> populate(Vec<Sys::CpuInfo>&);

Res<> populate(Sys::UserInfo&);

Res<> populate(Vec<Sys::UserInfo>&);

// MARK: Process Managment -----------------------------------------------------

Res<> sleep(Duration);

Res<> sleepUntil(Instant);

Res<> exit(i32);

Res<Ref::Url> pwd();

// MARK: Sandboxing ------------------------------------------------------------

Res<> hardenSandbox();

// MARK: Addr ------------------------------------------------------------------

Async::Task<Vec<Ip>> ipLookupAsync(Str host);

// MARK: Asynchronous I/O ------------------------------------------------------

Sched& globalSched();

// MARK: Bundle ----------------------------------------------------------------

Res<Vec<String>> installedBundles();

Res<String> currentBundle();

} // namespace Karm::Sys::_Embed
