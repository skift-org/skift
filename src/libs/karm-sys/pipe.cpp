#include "pipe.h"

#include "_embed.h"

namespace Karm::Sys {

Res<Pipe> Pipe::create() {
    auto pipe = try$(_Embed::createPipe());
    return Ok(Pipe{pipe.car, pipe.cdr});
}

} // namespace Karm::Sys
