#include "proc.h"

namespace Karm::Sys {

static bool _sandboxed = false;

void enterSandbox() {
    _Embed::hardenSandbox();
    _sandboxed = true;
}

Res<> ensureUnrestricted() {
    if (_sandboxed)
        return Error::permissionDenied("sandboxed");
    return Ok();
}

} // namespace Karm::Sys
