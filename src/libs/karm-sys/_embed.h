#pragma once

#include <karm-base/cons.h>
#include <karm-base/range.h>
#include <karm-base/time.h>
#include <karm-mime/uti.h>

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

Res<Strong<Sys::Fd>> unpackFd(Io::PackScan &);

// MARK: File I/O --------------------------------------------------------------

Res<Strong<Sys::Fd>> openFile(Mime::Url const &url);

Res<Strong<Sys::Fd>> createFile(Mime::Url const &url);

Res<Strong<Sys::Fd>> openOrCreateFile(Mime::Url const &url);

Res<Cons<Strong<Sys::Fd>, Strong<Sys::Fd>>> createPipe();

Res<Strong<Sys::Fd>> createIn();

Res<Strong<Sys::Fd>> createOut();

Res<Strong<Sys::Fd>> createErr();

Res<Vec<Sys::DirEntry>> readDir(Mime::Url const &url);

Res<Stat> stat(Mime::Url const &url);

// MARK: User interactions -----------------------------------------------------

Res<> launch(Intent intent);

Async::Task<> launchAsync(Intent intent);

// MARK: Sockets ---------------------------------------------------------------

Res<Strong<Sys::Fd>> listenUdp(SocketAddr addr);

Res<Strong<Sys::Fd>> connectTcp(SocketAddr addr);

Res<Strong<Sys::Fd>> listenTcp(SocketAddr addr);

Res<Strong<Sys::Fd>> listenIpc(Mime::Url url);

// MARK: Time ------------------------------------------------------------------

TimeStamp now();

TimeSpan uptime();

// MARK: Memory Managment ------------------------------------------------------

Res<Sys::MmapResult> memMap(Sys::MmapOptions const &options);

Res<Sys::MmapResult> memMap(Sys::MmapOptions const &options, Strong<Sys::Fd> fd);

Res<> memUnmap(void const *buf, usize len);

Res<> memFlush(void *flush, usize len);

// MARK: System Informations ---------------------------------------------------

Res<> populate(Sys::SysInfo &);

Res<> populate(Sys::MemInfo &);

Res<> populate(Vec<Sys::CpuInfo> &);

Res<> populate(Sys::UserInfo &);

Res<> populate(Vec<Sys::UserInfo> &);

// MARK: Process Managment -----------------------------------------------------

Res<> sleep(TimeSpan);

Res<> sleepUntil(TimeStamp);

Res<> exit(i32);

// MARK: Sandboxing ------------------------------------------------------------

void hardenSandbox();

// MARK: Asynchronous I/O ------------------------------------------------------

Sched &globalSched();

} // namespace Karm::Sys::_Embed
