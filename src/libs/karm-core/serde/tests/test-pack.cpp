#include <karm-test/macros.h>

import Karm.Core;

namespace Karm::Serde::Tests {

test$("pack-unpack-primitives") {
    auto packUnpack = [&]<typename T>(T expected) -> Res<> {
        Io::BufferWriter buf;
        Io::BEmit emit{buf};
        PackSerializer ser{emit};
        try$(serialize(ser, expected));
        Io::BScan scan{buf.bytes()};
        PackDeserializer de{scan};
        auto actual = try$(de.deserialize<T>());

        expectEq$(expected, actual);

        return Ok();
    };

    try$(packUnpack(NONE));
    try$(packUnpack(Opt<float>{NONE}));
    try$(packUnpack(Opt<float>{3.14}));

    try$(packUnpack(Res<String>{Ok("Hello"s)}));
    try$(packUnpack(Res<String>{Error::invalidData()}));

    using U = Union<int, String, bool>;
    try$(packUnpack(U{3}));
    try$(packUnpack(U{String{"wow such union"s}}));
    try$(packUnpack(U{false}));
    try$(packUnpack(U{true}));

    try$(packUnpack(true));
    try$(packUnpack(false));

    try$(packUnpack(0));
    try$(packUnpack(100));
    try$(packUnpack(-100));

    try$(packUnpack(0.0));
    try$(packUnpack(100.0));
    try$(packUnpack(-100.0));

    try$(packUnpack(String{"Hello, world!"s}));

    Vec<u8> bytes{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    try$(packUnpack(bytes));

    Tuple<int, float, String> tuple{1, 0.99, "wow"s};
    try$(packUnpack(tuple));

    struct Foo {
        int a;
        float b;
        String c;

        bool operator==(Foo const&) const = default;

        void repr(Io::Emit& e) const {
            e("(foo {} {} {})", a, b, c);
        }
    } foo = {42, 69.420, "Nerds"s};

    try$(packUnpack(foo));

    return Ok();
}

} // namespace Karm::Serde::Tests
