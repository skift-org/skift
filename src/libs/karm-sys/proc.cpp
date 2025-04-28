#include "proc.h"

namespace Karm::Sys {

static bool _sandboxed = false;

Res<> enterSandbox() {
    try$(_Embed::hardenSandbox());
    _sandboxed = true;
    return Ok();
}

Res<> ensureUnrestricted() {
    if (_sandboxed)
        return Error::permissionDenied("sandboxed");
    return Ok();
}

} // namespace Karm::Sys
