#pragma once

#include <karm-base/cons.h>
#include <karm-base/range.h>
#include <karm-base/time.h>

#include "dir.h"
#include "fd.h"
#include "info.h"
#include "types.h"

namespace Karm::Sys::_Embed {

/* --- File I/O ------------------------------------------------------------- */

Res<Strong<Sys::Fd>> openFile(Url::Url const &url);

Res<Strong<Sys::Fd>> createFile(Url::Url const &url);

Res<Strong<Sys::Fd>> openOrCreateFile(Url::Url const &url);

Res<Cons<Strong<Sys::Fd>, Strong<Sys::Fd>>> createPipe();

Res<Strong<Sys::Fd>> createIn();

Res<Strong<Sys::Fd>> createOut();

Res<Strong<Sys::Fd>> createErr();

Res<Vec<Sys::DirEntry>> readDir(Url::Url const &url);

Res<Stat> stat(Url::Url const &url);

/* --- Sockets -------------------------------------------------------------- */

Res<Strong<Sys::Fd>> listenUdp(SocketAddr addr);

Res<Strong<Sys::Fd>> connectTcp(SocketAddr addr);

Res<Strong<Sys::Fd>> listenTcp(SocketAddr addr);

/* --- Time ----------------------------------------------------------------- */

TimeStamp now();

TimeSpan uptime();

/* --- Memory Managment ----------------------------------------------------- */

Res<Sys::MmapResult> memMap(Sys::MmapOptions const &options);

Res<Sys::MmapResult> memMap(Sys::MmapOptions const &options, Strong<Sys::Fd> fd);

Res<> memUnmap(void const *buf, usize len);

Res<> memFlush(void *flush, usize len);

/* --- System Informations -------------------------------------------------- */

Res<> populate(Sys::SysInfo &);

Res<> populate(Sys::MemInfo &);

Res<> populate(Vec<Sys::CpuInfo> &);

Res<> populate(Sys::UserInfo &);

Res<> populate(Vec<Sys::UserInfo> &);

/* --- Process Managment ---------------------------------------------------- */

Res<> sleep(TimeSpan);

Res<> exit(i32);

} // namespace Karm::Sys::_Embed
