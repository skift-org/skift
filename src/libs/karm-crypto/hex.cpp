#include <karm-io/fmt.h>

#include "hex.h"

namespace Karm::Crypto {

Res<usize> hexEncode(Bytes bytes, Io::TextWriter &out) {
    usize written = 0;
    for (auto b : bytes) {
        written += try$(Io::format(out, "{02x}"s, b));
    }
    return Ok(written);
}

Res<String> hexEncode(Bytes bytes) {
    Io::StringWriter out;
    try$(hexEncode(bytes, out));
    return Ok(out.str());
}

} // namespace Karm::Crypto
