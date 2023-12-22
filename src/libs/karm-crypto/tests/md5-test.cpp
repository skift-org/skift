#include <karm-crypto/md5.h>
#include <karm-test/macros.h>

namespace Karm::Crypto::Tests {

test$(md5Hash) {
    Md5 md5;

    Str text1 = "";
    Md5::Digest hash1 = {0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e};

    md5.reset();
    md5.add(bytes(text1));
    expectEq$(md5.digest(), hash1);

    Str text2 = "abc";
    Md5::Digest hash2 = {0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0, 0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72};

    md5.reset();
    md5.add(bytes(text2));
    expectEq$(md5.digest(), hash2);

    Str text3_1 = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcde"};
    Str text3_2 = {"fghijklmnopqrstuvwxyz0123456789"};
    Md5::Digest hash3 = {0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5, 0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f};

    md5.reset();
    md5.add(bytes(text3_1));
    md5.add(bytes(text3_2));
    expectEq$(md5.digest(), hash3);

    return Ok();
}

} // namespace Karm::Crypto::Tests
