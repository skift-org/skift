#include "context.h"

namespace Karm::Sys {

Context &globalContext() {
    static Context ctx;
    return ctx;
}

} // namespace Karm::Sys
