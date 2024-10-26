#include "launch.h"

#include "_embed.h"

namespace Karm::Sys {

Res<> launch(Mime::Uti const &uti, Mime::Url const &url) {
    return _Embed::launch(uti, url);
}

Async::Task<> launchAsync(Mime::Uti const &uti, Mime::Url const &url) {
    return _Embed::launchAsync(uti, url);
}

} // namespace Karm::Sys
