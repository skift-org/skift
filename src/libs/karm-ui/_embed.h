#pragma once

#include <karm-sys/context.h>
#include <karm-ui/host.h>

namespace Karm::Ui::_Embed {

Res<Strong<Host>> makeHost(Child root);

Async::Task<> runAsync(Sys::Context &ctx, Child root);

} // namespace Karm::Ui::_Embed
