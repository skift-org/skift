#include <karm-crypto/crc32.h>
#include <karm-test/macros.h>

namespace Karm::Crypto::Tests {

test$("crypto-crc32") {
    auto testCase = [&](Str data, u32 expected) -> Res<> {
        auto crc = crc32(bytes(data));
        expectEq$(crc, expected);
        return Ok();
    };

    try$(testCase("", 0x0));
    try$(testCase("The quick brown fox jumps over the lazy dog", 0x414FA339));
    try$(testCase("various CRC algorithms input data", 0x9BD366AE));

    return Ok();
}

} // namespace Karm::Crypto::Tests
