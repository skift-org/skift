#pragma once

#include <karm-io/traits.h>

namespace Karm::Crypto {

Res<usize> hexEncode(Bytes bytes, Io::TextWriter& out);

Res<String> hexEncode(Bytes bytes);

} // namespace Karm::Crypto
