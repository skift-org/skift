#include "stat.h"

#include "_embed.h"

namespace Karm::Sys {

Res<Stat> stat(Url::Url const &url) {
    return _Embed::stat(url);
}

} // namespace Karm::Sys
