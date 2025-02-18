#include <karm-io/fmt.h>

#include "hex.h"

namespace Karm::Crypto {

Res<> hexEncode(Bytes bytes, Io::TextWriter& out) {
    for (auto b : bytes) {
        try$(Io::format(out, "{02x}"s, b));
    }
    return Ok();
}

Res<String> hexEncode(Bytes bytes) {
    Io::StringWriter out;
    try$(hexEncode(bytes, out));
    return Ok(out.str());
}

} // namespace Karm::Crypto
