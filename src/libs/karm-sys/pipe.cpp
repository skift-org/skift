#include <embed/sys.h>

#include "pipe.h"

namespace Karm::Sys {

Result<Pipe> Pipe::create() {
    auto pipe = try$(Embed::createPipe());
    return Pipe{pipe.car, pipe.cdr};
}

} // namespace Karm::Sys
