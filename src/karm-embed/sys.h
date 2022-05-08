#pragma once

#include <karm-base/cons.h>
#include <karm-sys/fd.h>

namespace Karm::Embed {

Base::Result<Base::Strong<Sys::Fd>> openFile(Sys::Path path);

Base::Result<Base::Strong<Sys::Fd>> createFile(Sys::Path path);

Base::Result<Base::Cons<Base::Strong<Sys::Fd>, Base::Strong<Sys::Fd>>> createPipe();

Base::Result<Base::Strong<Sys::Fd>> createIn();

Base::Result<Base::Strong<Sys::Fd>> createOut();

Base::Result<Base::Strong<Sys::Fd>> createErr();

} // namespace Karm::Embed
