#include "_embed.h"
#include "addr.h"

namespace Karm::Sys {

Async::Task<Vec<Ip>> lookupAsync(Str host) {
    return _Embed::ipLookupAsync(host);
}

} // namespace Karm::Sys
