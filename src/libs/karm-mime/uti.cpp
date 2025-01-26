#include "uti.h"

#include "mime.h"

namespace Karm::Mime {

Res<Uti> Uti::fromMime(Mime const& mime) {
#define UTI(NAME, STR, MIME, ...) \
    if (mime.is(Mime{MIME}))      \
        return Ok(Uti::NAME);
#include "defs/uti.inc"
#undef UTI

    return Error::invalidData("enknown mime type");
}

} // namespace Karm::Mime
