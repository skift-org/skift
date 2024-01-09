#include "context.h"

namespace Karm::Sys {

Ctx &globalCtx() {
    static Ctx ctx;
    return ctx;
}

} // namespace Karm::Sys
