#pragma once

#include <karm-base/cons.h>
#include <karm-base/range.h>
#include <karm-base/time.h>
#include <karm-sys/dir.h>
#include <karm-sys/fd.h>
#include <karm-sys/info.h>
#include <karm-sys/types.h>

#include "defs.h"

namespace Karm::Sys::_Embed {

/* --- File I/O ------------------------------------------------------------- */

Res<Strong<Sys::Fd>> openFile(Url::Url url);

Res<Strong<Sys::Fd>> createFile(Url::Url url);

Res<Cons<Strong<Sys::Fd>, Strong<Sys::Fd>>> createPipe();

Res<Strong<Sys::Fd>> createIn();

Res<Strong<Sys::Fd>> createOut();

Res<Strong<Sys::Fd>> createErr();

Res<Vec<Sys::DirEntry>> readDir(Url::Url url);

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
