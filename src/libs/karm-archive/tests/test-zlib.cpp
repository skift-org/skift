#include <karm-test/macros.h>

import Karm.Archive;

namespace Karm::Archive::Tests {

test$("zlib-decompress") {
    auto testCase = [&](Bytes input, Str expected) -> Res<> {
        auto res = try$(zlibDecompress(input));
        expectEq$(res, bytes(expected));
        return Ok();
    };

    Array input0 = {static_cast<u8>(120), 1, 1, 12, 0, 243, 255, 72, 101, 108, 108, 111, 32, 87, 111, 114, 108, 100, 33, 28, 73, 4, 62};
    try$(testCase(input0, "Hello World!"));
    Array input1 = {static_cast<u8>(120), 218, 85, 145, 107, 106, 3, 49, 12, 132, 175, 50, 7, 88, 114, 146, 22, 90, 122, 2, 197, 171, 36, 2, 191, 162, 71, 232, 241, 43, 111, 247, 71, 11, 6, 99, 89, 158, 25, 125, 126, 27, 202, 13, 50, 45, 26, 246, 81, 135, 194, 196, 65, 141, 125, 67, 25, 221, 184, 56, 123, 40, 104, 151, 41, 86, 164, 223, 193, 85, 252, 130, 119, 226, 194, 157, 12, 157, 11, 134, 22, 1, 7, 170, 92, 89, 199, 122, 184, 75, 227, 238, 41, 74, 165, 68, 51, 234, 23, 124, 60, 200, 184, 214, 48, 240, 55, 154, 108, 41, 196, 103, 211, 210, 72, 203, 117, 149, 190, 26, 174, 89, 20, 42, 81, 197, 208, 40, 84, 236, 239, 251, 38, 232, 209, 203, 134, 89, 169, 176, 146, 67, 118, 84, 30, 240, 108, 116, 121, 6, 111, 136, 154, 135, 194, 6, 227, 54, 89, 225, 67, 115, 93, 240, 197, 123, 150, 246, 211, 238, 37, 47, 86, 165, 84, 122, 144, 178, 43, 193, 207, 136, 145, 181, 172, 72, 6, 201, 249, 231, 10, 240, 25, 98, 169, 13, 139, 4, 49, 19, 83, 104, 167, 28, 16, 220, 165, 253, 163, 229, 233, 185, 204, 222, 143, 232, 199, 120, 207, 160, 132, 17, 247, 224, 51, 3, 221, 197, 93, 108, 59, 231, 203, 40, 78, 12, 190, 221, 164, 200, 210, 176, 146, 124, 244, 112, 76, 84, 230, 103, 226, 153, 249, 18, 26, 31, 73, 94, 235, 47, 182, 181, 225, 70, 81, 228, 250, 11, 215, 104, 10, 119, 36, 151, 147, 221, 15, 116, 124, 179, 125};
    try$(testCase(input1, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas nec orci eu libero condimentum accumsan. Phasellus ex mi, elementum nec metus et, rutrum iaculis mauris. Phasellus mi nunc, placerat id leo tristique, ultrices semper tortor. Sed sed metus viverra, pharetra tellus eu, pretium turpis. Quisque suscipit urna ac enim consectetur tempor. Mauris nec quam augue. Sed sagittis, mauris vitae efficitur scelerisque, est metus pellentesque velit, vel faucibus ex sapien at mauris."));

    return Ok();
}

test$("zlib-empty-uncompressed") {
    Array input = {
        (u8)0x78, 0x9C, 0x01, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00,
        0x01
    };
    expectEq$(try$(zlibDecompress(input)).len(), 0uz);
    return Ok();
}

test$("zlib-empty-fixed") {
    Array input = {
        (u8)0x78, 0x9C, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01
    };
    expectEq$(try$(zlibDecompress(input)).len(), 0uz);
    return Ok();
}

test$("zlib-empty-dynamic") {
    Array input = {
        (u8)0x78, 0x9C, 0x05, 0xC1, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0xFF, 0xD5, 0x08, 0x00, 0x00, 0x00, 0x01
    };
    expectEq$(try$(zlibDecompress(input)).len(), 0uz);
    return Ok();
}

test$("zlib-one-byte-raw") {
    Array input = {
        (u8)0x78, 0x9C, 0x01, 0x01, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0x01,
        0x00, 0x01
    };
    auto out = try$(zlibDecompress(input));
    expectEq$(out.len(), 1uz);
    expectEq$(out[0], 0);

    return Ok();
}

test$("zlib-one-byte-fixed") {
    Array input = {
        (u8)0x78, 0x9C, 0x63, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01
    };
    auto out = try$(zlibDecompress(input));
    expectEq$(out.len(), 1uz);
    expectEq$(out[0], 0);

    return Ok();
}

test$("zlib-one-byte-dynamic") {
    Array input = {
        (u8)0x78, 0x9C, 0x05, 0xC1, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0xFF, 0xD5, 0x10, 0x00, 0x01, 0x00, 0x01
    };
    auto out = try$(zlibDecompress(input));
    expectEq$(out.len(), 1uz);
    expectEq$(out[0], 0);

    return Ok();
}

test$("zlib-zeroes") {
    Array input = {
        (u8)0x78, 0x9C, 0x63, 0x60, 0x18, 0xD9, 0x00, 0x00, 0x01, 0x00,
        0x00, 0x01
    };

    auto out = try$(zlibDecompress(input));
    expectEq$(out.len(), 256uz);
    for (usize i : range(256)) {
        expectEq$(out[i], 0);
    }

    return Ok();
}

} // namespace Karm::Archive::Tests