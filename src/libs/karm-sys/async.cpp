#include "async.h"

#include "_embed.h"

namespace Karm::Sys {

Sched& globalSched() {
    return _Embed::globalSched();
}

} // namespace Karm::Sys
