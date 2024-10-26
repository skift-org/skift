#include <karm-logger/logger.h>

#include "_embed.h"

#include "async.h"

namespace Karm::Sys {

Sched &globalSched() {
    return _Embed::globalSched();
}

} // namespace Karm::Sys
