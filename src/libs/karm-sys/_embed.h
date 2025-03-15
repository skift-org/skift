#pragma once

#include <karm-base/range.h>
#include <karm-base/time.h>
#include <karm-base/tuple.h>
#include <karm-mime/uti.h>

#include "async.h"
#include "dir.h"
#include "fd.h"
#include "info.h"
#include "types.h"
#include "addr.h"

namespace Karm::Sys {

struct Intent;

} // namespace Karm::Sys

namespace Karm::Sys::_Embed {

// MARK: Fd --------------------------------------------------------------------

Res<Rc<Sys::Fd>> unpackFd(Io::PackScan&);

// MARK: File I/O --------------------------------------------------------------

Res<Rc<Sys::Fd>> openFile(Mime::Url const& url);

Res<Rc<Sys::Fd>> createFile(Mime::Url const& url);

Res<Rc<Sys::Fd>> openOrCreateFile(Mime::Url const& url);

Res<Pair<Rc<Sys::Fd>, Rc<Sys::Fd>>> createPipe();

Res<Rc<Sys::Fd>> createIn();

Res<Rc<Sys::Fd>> createOut();

Res<Rc<Sys::Fd>> createErr();

Res<Vec<Sys::DirEntry>> readDir(Mime::Url const& url);

Res<Stat> stat(Mime::Url const& url);

// MARK: User interactions -----------------------------------------------------

Res<> launch(Intent intent);

Async::Task<> launchAsync(Intent intent);

// MARK: Sockets ---------------------------------------------------------------

Res<Rc<Sys::Fd>> listenUdp(SocketAddr addr);

Res<Rc<Sys::Fd>> connectTcp(SocketAddr addr);

Res<Rc<Sys::Fd>> listenTcp(SocketAddr addr);

Res<Rc<Sys::Fd>> listenIpc(Mime::Url url);

// MARK: Time ------------------------------------------------------------------

SystemTime now();

Instant instant();

Duration uptime();

// MARK: Memory Managment ------------------------------------------------------

Res<Sys::MmapResult> memMap(Sys::MmapOptions const& options);

Res<Sys::MmapResult> memMap(Sys::MmapOptions const& options, Rc<Sys::Fd> fd);

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

Res<Mime::Url> pwd();

// MARK: Sandboxing ------------------------------------------------------------

void hardenSandbox();

// MARK: Addr ------------------------------------------------------------------

Async::Task<Vec<Ip>> ipLookupAsync(Str host);

// MARK: Asynchronous I/O ------------------------------------------------------

Sched& globalSched();

} // namespace Karm::Sys::_Embed
