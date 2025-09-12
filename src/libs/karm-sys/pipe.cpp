#include "pipe.h"

#include "_embed.h"
#include "proc.h"

namespace Karm::Sys {

Res<Pipe> Pipe::create() {
    try$(ensureUnrestricted());
    auto pipe = try$(_Embed::createPipe());
    return Ok(Pipe{
        FileWriter{pipe.v0, "pipe:"_url},
        FileReader{pipe.v1, "pipe:"_url},
    });
}

} // namespace Karm::Sys
