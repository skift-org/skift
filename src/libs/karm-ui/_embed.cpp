module;

#include <karm-sys/context.h>

export module Karm.Ui:_embed;

import :node;

namespace Karm::Ui::_Embed {

export Async::Task<> runAsync(Sys::Context&, Child root);

} // namespace Karm::Ui::_Embed
