#include "pipe.h"

#include "_embed.h"

#include "proc.h"

namespace Karm::Sys {

Res<Pipe> Pipe::create() {
    try$(ensureUnrestricted());
    auto pipe = try$(_Embed::createPipe());
    return Ok(Pipe{
        FileWriter{pipe.car, "pipe:"_url},
        FileReader{pipe.cdr, "pipe:"_url},
    });
}

} // namespace Karm::Sys
