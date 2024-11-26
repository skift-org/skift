#include "launch.h"

#include "_embed.h"

#include "proc.h"

namespace Karm::Sys {

Res<> launch(Intent intent) {
    try$(ensureUnrestricted());
    return _Embed::launch(intent);
}

Async::Task<> launchAsync(Intent intent) {
    co_try$(ensureUnrestricted());
    co_return co_await _Embed::launchAsync(intent);
}

} // namespace Karm::Sys
