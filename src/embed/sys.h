#pragma once

#include <karm-base/cons.h>
#include <karm-base/range.h>
#include <karm-sys/fd.h>
#include <karm-sys/types.h>

namespace Embed {

Result<Strong<Sys::Fd>> openFile(Sys::Path path);

Result<Strong<Sys::Fd>> createFile(Sys::Path path);

Result<Cons<Strong<Sys::Fd>, Strong<Sys::Fd>>> createPipe();

Result<Strong<Sys::Fd>> createIn();

Result<Strong<Sys::Fd>> createOut();

Result<Strong<Sys::Fd>> createErr();

Result<Sys::MmapResult> memMap(Sys::MmapOptions const &options);

Result<Sys::MmapResult> memMap(Sys::MmapOptions const &options, Strong<Sys::Fd> fd);

Error memUnmap(void const *buf, size_t len);

Error memFlush(void *flush, size_t len);

#ifdef __osdk_encoding_utf8__
using Encoding = Utf8;
#elifdef __osdk_encoding_utf16__
using Encoding = Utf16;
#endif

} // namespace Embed
