#include "lookup.h"

#include "_embed.h"
#include "addr.h"

namespace Karm::Sys {

[[clang::coro_wrapper]]
Async::Task<Vec<Ip>> lookupAsync(Str host) {
    return _Embed::ipLookupAsync(host);
}

} // namespace Karm::Sys
