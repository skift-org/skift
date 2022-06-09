#pragma once

namespace Embed {

void debugHandler(char const* buf);

[[noreturn]] void panicHandler(char const *buf);

} // namespace Embed
