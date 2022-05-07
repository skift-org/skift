#pragma once

#include <karm-base/cons.h>
#include <karm-sys/fd.h>

namespace Embed {

Karm::Base::Result<Karm::Base::Strong<Karm::Sys::Fd>> openFile(Karm::Sys::Path path);

Karm::Base::Result<Karm::Base::Strong<Karm::Sys::Fd>> createFile(Karm::Sys::Path path);

Karm::Base::Result<Karm::Base::Cons<Karm::Base::Strong<Karm::Sys::Fd>, Karm::Base::Strong<Karm::Sys::Fd>>> createPipe();

Karm::Base::Result<Karm::Base::Strong<Karm::Sys::Fd>> createIn();

Karm::Base::Result<Karm::Base::Strong<Karm::Sys::Fd>> createOut();

Karm::Base::Result<Karm::Base::Strong<Karm::Sys::Fd>> createErr();

} // namespace Embed
