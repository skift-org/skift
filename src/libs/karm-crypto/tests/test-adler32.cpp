#include <karm-test/macros.h>

import Karm.Crypto;

namespace Karm::Crypto::Tests {

test$("crypto-adler32") {
    auto testCase = [&](Str data, u32 expected) -> Res<> {
        auto adler = adler32(bytes(data));
        expectEq$(adler, expected);
        return Ok();
    };

    try$(testCase("", 0x1));
    try$(testCase("a", 0x00620062));
    try$(testCase("abc", 0x024d0127));
    try$(testCase("message digest", 0x29750586));
    try$(testCase("abcdefghijklmnopqrstuvwxyz", 0x90860b20));

    return Ok();
}

} // namespace Karm::Crypto::Tests
