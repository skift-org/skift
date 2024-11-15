#include "launch.h"

#include "_embed.h"

#include "proc.h"

namespace Karm::Sys {

Res<> launch(Mime::Uti const &uti, Mime::Url const &url) {
    try$(ensureUnrestricted());
    return _Embed::launch(uti, url);
}

Async::Task<> launchAsync(Mime::Uti const &uti, Mime::Url const &url) {
    co_try$(ensureUnrestricted());
    co_return co_await _Embed::launchAsync(uti, url);
}

} // namespace Karm::Sys
