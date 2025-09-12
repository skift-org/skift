#include <karm-test/macros.h>

import Karm.Ml;

namespace Karm::Ml::Tests {

test$("bpe-byte-encode") {
    for (u8 const r : range(255)) {
        expectEq$(BpeVocab::_unicodeToU8(BpeVocab::_u8ToUnicode(r)), r);
    }

    return Ok();
}

} // namespace Karm::Ml::Tests
