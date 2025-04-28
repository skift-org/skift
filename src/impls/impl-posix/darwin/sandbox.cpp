#include <karm-logger/logger.h>
#include <karm-sys/_embed.h>

namespace Karm::Sys::_Embed {

Res<> hardenSandbox() {
    logError("sandbox hardening is not supported on macOS");
    return Ok();
}

} // namespace Karm::Sys::_Embed
