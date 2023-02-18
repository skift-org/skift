#include <embed-sys/sys.h>

#include "pipe.h"

namespace Karm::Sys {

Res<Pipe> Pipe::create() {
    auto pipe = try$(Embed::createPipe());
    return Ok(Pipe{pipe.car, pipe.cdr});
}

} // namespace Karm::Sys
