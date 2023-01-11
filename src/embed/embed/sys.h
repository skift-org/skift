#pragma once

#include <karm-base/cons.h>
#include <karm-base/range.h>
#include <karm-sys/dir.h>
#include <karm-sys/fd.h>
#include <karm-sys/info.h>
#include <karm-sys/types.h>

#include "sys-defs.h"

namespace Embed {

Result<Strong<Sys::Fd>> openFile(Sys::Path path);

Result<Vec<Sys::DirEntry>> readDir(Sys::Path path);

Result<Strong<Sys::Fd>> createFile(Sys::Path path);

Result<Cons<Strong<Sys::Fd>, Strong<Sys::Fd>>> createPipe();

Result<Strong<Sys::Fd>> createIn();

Result<Strong<Sys::Fd>> createOut();

Result<Strong<Sys::Fd>> createErr();

/* --- Memory Managment ----------------------------------------------------- */

Result<Sys::MmapResult> memMap(Sys::MmapOptions const &options);

Result<Sys::MmapResult> memMap(Sys::MmapOptions const &options, Strong<Sys::Fd> fd);

Error memUnmap(void const *buf, size_t len);

Error memFlush(void *flush, size_t len);

/* --- System Informations -------------------------------------------------- */

Error populate(Sys::SysInfo &);

Error populate(Sys::MemInfo &);

Error populate(Vec<Sys::CpuInfo> &);

Error populate(Sys::UserInfo &);

Error populate(Vec<Sys::UserInfo> &);

} // namespace Embed
