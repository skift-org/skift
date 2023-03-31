#pragma once

#include <karm-base/cons.h>
#include <karm-base/range.h>
#include <karm-base/time.h>
#include <karm-sys/dir.h>
#include <karm-sys/fd.h>
#include <karm-sys/info.h>
#include <karm-sys/types.h>

#include "defs.h"

namespace Embed {

Res<Strong<Sys::Fd>> openFile(Sys::Path path);

Res<Vec<Sys::DirEntry>> readDir(Sys::Path path);

Res<Strong<Sys::Fd>> createFile(Sys::Path path);

Res<Cons<Strong<Sys::Fd>, Strong<Sys::Fd>>> createPipe();

Res<Strong<Sys::Fd>> createIn();

Res<Strong<Sys::Fd>> createOut();

Res<Strong<Sys::Fd>> createErr();

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

} // namespace Embed
