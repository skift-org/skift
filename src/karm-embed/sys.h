#pragma once

#include <karm-base/cons.h>
#include <karm-sys/fd.h>

namespace Karm::Embed {

Result<Strong<Sys::Fd>> openFile(Sys::Path path);

Result<Strong<Sys::Fd>> createFile(Sys::Path path);

Result<Cons<Strong<Sys::Fd>, Strong<Sys::Fd>>> createPipe();

Result<Strong<Sys::Fd>> createIn();

Result<Strong<Sys::Fd>> createOut();

Result<Strong<Sys::Fd>> createErr();

} // namespace Karm::Embed
