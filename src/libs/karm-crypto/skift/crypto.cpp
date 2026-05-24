module Karm.Crypto;

import Karm.Core;

namespace Karm::Crypto::_Embed {

Res<> entropy(MutBytes) {
    // FIXME: Implement a proper entropy source
    return Ok();
}

} // namespace Karm::Crypto::_Embed
