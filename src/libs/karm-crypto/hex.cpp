module;

#include <karm-core/macros.h>

export module Karm.Crypto:hex;

import Karm.Core;

namespace Karm::Crypto {

export Res<> hexEncode(Bytes bytes, Io::TextWriter& out) {
    for (auto b : bytes) {
        try$(Io::format(out, "{02x}"s, b));
    }
    return Ok();
}

export Res<String> hexEncode(Bytes bytes) {
    Io::StringWriter out;
    try$(hexEncode(bytes, out));
    return Ok(out.str());
}

} // namespace Karm::Crypto
